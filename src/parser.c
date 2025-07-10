///////////////
// Parser
void parser_init(Parser* parser, Lexer* lexer) {
  MemoryZeroStruct(parser);

  parser->arena = arena_init();
  parser->lexer = lexer;
  parser->error = (Parser_Error){ Str8(""), false, 0, 0 };
  parser->root  = ast_node_new(parser, 0, 0, AST_Node_Program);

  lexer_get_next_token(lexer);
}

AST_Node* parser_parse_file(Parser* parser) {

  while (parser->lexer->current_token.type != Token_End_Of_File) {
    parser_parse_whitespace(parser, parser->root);

    // Top level constructs. Order matters.
    if (0)    {
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
  Token first_token = parser->lexer->current_token;
  if (first_token.type != Token_Space && first_token.type != Token_Tab && first_token.type != Token_New_Line) {
    return;
  }
  
  AST_Node_Type node_type = (first_token.type == Token_Space) ? AST_Node_Space :
                            (first_token.type == Token_Tab)   ? AST_Node_Tab : AST_Node_New_Line;
  
  u32 start_offset = first_token.start_offset;
  u32 end_offset   = first_token.end_offset;
  
  //parser_advance(parser);
  
  // Consume consecutive tokens of same type
  while (parser->lexer->current_token.type == first_token.type) {
    end_offset = parser->lexer->current_token.end_offset;
    parser_advance(parser);
  }
  
  AST_Node* node = ast_node_new(parser, start_offset, end_offset, node_type);
  ast_node_add_child(parser, parent, node);
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

  /*
  if (parser->lexer->current_token.type == Token_Comment_Line) {
    Token_Array tokens = token_array_new(parser->arena, MAX_TOKENS_IN_ARRAY); 
    Token current_token = parser->lexer->current_token;

    while (current_token.type != Token_New_Line) {
      tokens.tokens[tokens.count++] = current_token;
      parser_advance(parser);
      current_token = parser->lexer->current_token;
    }

    ast_node_add_child(parser, parser->root, ast_node_new(parser, tokens, AST_Node_Comment_Line));
    result = true;
  }
  */
  return result;
}

b32 parser_parse_comment_block(Parser* parser) {
  b32 result = false;
  /*
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
  */
  return result;
}

b32 parser_parse_preprocessor_directives(Parser* parser) {
  b32 result = false;
  /*
  Arena_Temp scratch = scratch_begin(0,0);
  if (parser->lexer->current_token.type == Token_Preprocessor_Hash) {
    parser_advance(parser);
    Token preprocessor_type = parser->lexer->current_token;
    parser_parse_whitespace(parser, parser->root);
      
    // Include directive
    if (string8_equal(preprocessor_type.value, Str8("include"))) {
      parser_advance(parser);
      parser_parse_whitespace(parser, parser->root);
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
  */
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

AST_Node* ast_node_new(Parser* parser, u32 start_offset, u32 end_offset, AST_Node_Type type) {
  AST_Node* node       = arena_push(parser->arena, sizeof(AST_Node));
  node->start_offset   = start_offset;
  node->end_offset     = end_offset;
  node->type           = type;
  node->children       = 0;
  node->children_count = 0;
  return node;
}

void ast_node_add_child(Parser* parser, AST_Node* parent, AST_Node* child) {
  if (parent->children_count == 0) {
    parent->children = ArenaPush(parser->arena, AST_Node*, 4);
  }
  
  if (parent->children_count > 0 && (parent->children_count & (parent->children_count - 1)) == 0) {
    u32 new_capacity = parent->children_count * 2;
    // NOTE(fz): If we do a lot of reallocations, we could be filling the arena with unused memory
    AST_Node** new_children = ArenaPush(parser->arena, AST_Node*, new_capacity);
    for (u32 i = 0; i < parent->children_count; i += 1) {
      new_children[i] = parent->children[i];
    }
    parent->children = new_children;
  }
  
  parent->children[parent->children_count++] = child;
}

void parser_print_ast(Parser* parser, b32 print_whitespace, b32 print_comments) {
  parser_print_ast_node(parser, parser->root, 0, print_whitespace, print_comments);
}

void parser_print_ast_node(Parser* parser, AST_Node* node, u32 indent, b32 print_whitespace, b32 print_comments) {
  if (!node) return;
  if ((node->type == AST_Node_Space || node->type == AST_Node_Tab || node->type == AST_Node_New_Line) && !print_whitespace) return;
  if ((node->type == AST_Node_Comment_Line || node->type == AST_Node_Comment_Block) && !print_comments) return;

  Terminal_Color color = Terminal_Color_Default;
  switch (node->type) {
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

  for (u32 i = 0; i < indent; ++i) {
    printf("  ");
  }
  
  u32 size = node->end_offset - node->start_offset;
  printf_color(color, "{%s, %d}: '%.*s'", ast_node_types[node->type], size, size, (node->type != AST_Node_New_Line) ? (parser->lexer->file.data.str + node->start_offset) : "\\n");
  printf("\n");
  
  for (u32 i = 0; i < node->children_count; i += 1) {
    parser_print_ast_node(parser, node->children[i], indent + 1, print_whitespace, print_comments);
  }
}