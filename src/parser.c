///////////////
// Parser
void parser_init(Parser* parser, Lexer* lexer) {
  MemoryZeroStruct(parser);
  parser->arena = arena_init();
  parser->lexer = lexer;
  lexer_get_next_token(lexer);

  AST_Node* program       = ArenaPush(parser->arena, AST_Node, 1);
  program->ast_type       = AST_Node_Program;
  program->tokens         = (Token_Array) { NULL, 0 };
  program->children       = ArenaPush(parser->arena, AST_Node*, AST_NODE_MAX_CHILDREN);
  program->children_count = 0;

  parser->root = program;
}

AST_Node* parser_parse_file(Parser* parser) {

  while (parser->lexer->current_token.type != Token_End_Of_File) {

    // Top level constructs. Order matters.
    if (0)    {
    } else if (parser_parse_whitespace(parser)) {
    } else if (parser_parse_comment_line(parser)) {
    } else if (parser_parse_comment_block(parser)) {
    } else if (parser_parse_preprocessor_directives(parser)) {
    } else if (parser_parse_typedef(parser)) {
    } else if (parser_parse_function_definition(parser)) {
    } else if (parser_parse_declaration(parser)) {
    } else {
      parser_advance(parser);
    }
  }
  return parser->root;
}

void parser_parse_whitespace(Parser* parser, AST_Node* parent) {
  Arena_Temp scratch = scratch_begin(0,0);
  Token_Array tokens = token_array_new(scratch.arena, 0);
  Token first_token  = parser->lexer->current_token;

  AST_Node_Type node_type = AST_Node_Unknown;
  switch(first_token.type) {
    case Token_Space:    { node_type = AST_Node_Space;    } break;
    case Token_Tab:      { node_type = AST_Node_Tab;      } break;
    case Token_New_Line: { node_type = AST_Node_New_Line; } break;
  }

  while (first_token.type == Token_Space || first_token.type == Token_Tab || first_token.type == Token_New_Line) {
    Token current_token = parser->lexer->current_token;
    if (first_token != current_token)  break;

    AST_Node* node = ast_node_new(parser, node_type, 
    ast_node_add_child(parser, parent, node);
    parser_advance(parser);
  }
  scratch_end(&scratch);
}

void parser_advance(Parser* parser) {
  lexer_get_next_token(parser->lexer);
}

b32 parser_expect_token(Parser* parser, Token_Type expected) {
  if (parser->lexer->current_token.type != expected) {
    return false;
  }
  parser_advance(parser);
  return true;
}

b32 parser_is_token_datatype(Parser* parser) {
  Token token = parser->lexer->current_token;
  b32 result  = (token.type == Token_Keyword_Int      ||
                 token.type == Token_Keyword_Float    ||
                 token.type == Token_Keyword_Double   ||
                 token.type == Token_Keyword_Char     ||
                 token.type == Token_Keyword_Signed   ||
                 token.type == Token_Keyword_Unsigned ||
                 token.type == Token_Keyword_Void);
  return result;
}

AST_Node* parser_parse_expression(Parser* parser) {
  AST_Node* result = NULL;
  return result;
}

b32 parser_parse_comment_line(Parser* parser) {
  b32 result = false;
  Arena_Temp scratch = scratch_begin(0,0);
  if (parser->lexer->current_token.type == Token_Comment_Line) {
    Token current = parser->lexer->current_token;
    String8_List list = string8_list_new(scratch.arena, current.value);
    while (current.type != Token_New_Line) {
      string8_list_push(scratch.arena, &list, current.value);
      parser_advance(parser);
      current = parser->lexer->current_token;
    }
    String8  comment_node_value = string8_list_join(parser->arena, &list);
    AST_Node* comment_line_node = ast_node_new(parser, AST_Node_Comment_Line, comment_node_value);
    ast_node_add_child(parser, parser->root, comment_line_node);
    parser_advance(parser);
    result = true;
  }
  scratch_end(&scratch);
  return result;
}

b32 parser_parse_comment_block(Parser* parser) {
  b32 result = false;
  Arena_Temp scratch = scratch_begin(0,0);
  if (parser->lexer->current_token.type == Token_Comment_Block_Start) {
    Token current = parser->lexer->current_token;
    String8_List list = string8_list_new(scratch.arena, current.value);
    while (current.type != Token_Comment_Block_End) {
      string8_list_push(scratch.arena, &list, current.value);
      parser_advance(parser);
      current = parser->lexer->current_token;
    }
    string8_list_push(scratch.arena, &list, current.value);
    parser_advance(parser);
    current = parser->lexer->current_token;
    String8  comment_node_value = string8_list_join(parser->arena, &list);
    AST_Node* comment_line_node = ast_node_new(parser, AST_Node_Comment_Block, comment_node_value);
    ast_node_add_child(parser, parser->root, comment_line_node);
    parser_advance(parser);
    result = true;
  }
  scratch_end(&scratch);
  return result;
}

b32 parser_parse_preprocessor_directives(Parser* parser) {
  b32 result = false;
  Arena_Temp scratch = scratch_begin(0,0);
  if (parser->lexer->current_token.type == Token_Preprocessor_Hash) {
    parser_advance(parser);
    Token preprocessor_type = parser->lexer->current_token;
    parser_skip_whitespace(parser, parser->root);
      
    // Include directive
    if (string8_equal(preprocessor_type.value, Str8("include"))) {
      parser_advance(parser);
      parser_skip_whitespace(parser, parser->root);
      Token next = parser->lexer->current_token;
      switch (next.type) {
        case Token_Less: {
          parser_advance(parser);
            
          Token current = parser->lexer->current_token;
          String8_List list = string8_list_new(scratch.arena, current.value);
          while (current.type != Token_Greater) {
            string8_list_push(scratch.arena, &list, current.value);
            parser_advance(parser);
            current = parser->lexer->current_token;
          }
          String8 header_name = string8_list_join(parser->arena, &list);
          AST_Node* node = ast_node_new(parser, AST_Node_Preprocessor_Include_System, header_name);
          ast_node_add_child(parser, parser->root, node);
          parser_advance(parser);
        } break;
        case Token_String_Literal: {
          Token header_name = parser->lexer->current_token;
          Assert(header_name.value.str[0] == '"' && header_name.value.str[header_name.value.size-1] == '"');
          header_name.value = string8_new(header_name.value.size-2, header_name.value.str+1);
          AST_Node* node = ast_node_from_token(parser, AST_Node_Preprocessor_Include_Local, header_name);
          ast_node_add_child(parser, parser->root, node);
          parser_advance(parser);
        } break;
        default: {
          // TODO(fz): Error unknown include type
        } break;
      }

    // Define directive
    } else if (string8_equal(preprocessor_type.value, Str8("define"))) {
      // TODO(Fz): Still unhandled
      parser_advance(parser);

    // Pragma directive
    } else if (string8_equal(preprocessor_type.value, Str8("pragma"))) {
      // TODO(fz): Still unhandled
      // We should start by implementing the parse_expression function that will work for most of these.
      parser_advance(parser);

    } else {
      // TODO(fz): Error unhandled preprocessor type
      parser_advance(parser);
    }
    result = true;
  }
  scratch_end(&scratch);
  return result;
}

b32 parser_parse_typedef(Parser* parser) {
  b32 result = false;
  return result;
}
b32 parser_parse_function_definition(Parser* parser) {
  b32 result = false;
  return result;
}
b32 parser_parse_declaration(Parser* parser) {
  b32 result = false;
  return result;
}

///////////////
// AST

AST_Node* ast_node_new(Parser* parser, Token_Array token_array, AST_Node_Type node_type, u32 variant) {
  AST_Node* node = ArenaPush(parser->arena, AST_Node, 1);
  node->tokens   = token_array;

#if DEBUG
  Arena_Temp scratch = scratch_begin(0,0);
  if (token_array.count > 0) {
    String8_List token_value_list = string8_list_new(scratch->arena, node->tokens[0].tokens.value);
    for (u32 i = i; i < node->tokens.count; i += 1) {
      string8_list_push(scratch->arena, &token_value_list, node->tokens[i].tokens.value);
    }
    node->value = string8_list_join(parser->arena, &token_value_list);
  }
  scratch_end(&scratch);
#endif

  node->ast_type = node_type;
  switch (node_type) {
    case AST_Node_Binary_Op: { node->binary_op = (AST_Binary_Op)variant; }        break;
    case AST_Node_Unary_Op:  { node->unary_op = (AST_Unary_Op)variant; }          break;
    case AST_Node_Literal:   { node->literal_type = (AST_Literal_Type)variant; }  break;
    default: { ERROR_MESSAGE_AND_EXIT("Unhandled node type %s", ast_node_types[node_type]); } break;
  }

  node->children       = ArenaPush(parser->arena, AST_Node*, AST_NODE_MAX_CHILDREN);
  node->children_count = 0;
  return node;
}

void ast_node_add_child(Parser* parser, AST_Node* parent, AST_Node* child) {
  if (parent->children_count < AST_NODE_MAX_CHILDREN) {
    parent->children[parent->children_count] = child;
    parent->children_count += 1;
  } else {
    ERROR_MESSAGE_AND_EXIT("AST_Node has too many children. Consider increasing AST_NODE_MAX_CHILDREN");
  }
}

void ast_print(AST_Node* root, b32 print_whitespace, b32 print_comments) {
  ast_print_node(root, 0, print_whitespace, print_comments);
}

void ast_print_node(AST_Node* node, u32 indent, b32 print_whitespace, b32 print_comments) {
  if (!node) return;
  if ((node->ast_type == AST_Node_Space || node->ast_type == AST_Node_Tab || node->ast_type == AST_Node_New_Line) && !print_whitespace) return;
  if ((node->ast_type == AST_Node_Comment_Line || node->ast_type == AST_Node_Comment_Block) && !print_comments) return;

  Terminal_Color color = Terminal_Color_Default;
  switch (node->ast_type) {
    case AST_Node_Space: 
    case AST_Node_Tab:
    case AST_Node_New_Line:{
      color = Terminal_Color_Gray;
    } break;

    case AST_Node_Comment_Line:
    case AST_Node_Comment_Block: {
      color = Terminal_Color_Yellow;
    } break;

    case AST_Node_Preprocessor_Define:
    case AST_Node_Preprocessor_Pragma:
    case AST_Node_Preprocessor_Include_System:
    case AST_Node_Preprocessor_Include_Local: {
      color = Terminal_Color_Magenta;
    } break;
  }

  // Print indentation
  for (u32 i = 0; i < indent; ++i) {
    printf("  ");
  }
  
  // Print node type and value
  if (node->value.size > 0) {
    printf_color(color, ": \"%.*s\"", (s32)node->value.size, node->value.str);
  }
  printf("\n");
  
  // Print children
  for (u32 i = 0; i < node->children_count; ++i) {
    ast_print_node(node->children[i], indent + 1, print_whitespace, print_comments);
  }
}