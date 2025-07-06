///////////////
// Parser
void parser_init(Parser* parser, Lexer* lexer) {
  MemoryZeroStruct(parser);
  parser->arena = arena_init();
  parser->lexer = lexer;
  lexer_get_next_token(lexer);
}

AST_Node* parser_parse_file(Parser* parser) {
  AST_Node* program       = ArenaPush(parser->arena, AST_Node, 1);
  program->ast_type       = Node_Type_Program;
  program->token_type     = Token_Unknown;
  program->value          = Str8("Program");
  program->children       = ArenaPush(parser->arena, AST_Node*, AST_NODE_MAX_CHILDREN);
  program->children_count = 0;

  while (parser->lexer->current_token.type != Token_End_Of_File) {
    Arena_Temp scratch = scratch_begin(0,0);

    if (parser->lexer->current_token.type == Token_Spaces ||
        parser->lexer->current_token.type == Token_Tabs ||
        parser->lexer->current_token.type == Token_New_Line) {
      AST_Node* whitespace_node = ast_node_from_token(parser, Node_Type_Whitespace, parser->lexer->current_token);
      ast_node_add_child(parser, program, whitespace_node);
      parser_advance(parser);
      
    } else if (parser->lexer->current_token.type == Token_Comment_Line) {
      Token current = parser->lexer->current_token;
      parser_advance(parser);
      String8_List list = string8_list_new(scratch.arena, current.value);
      while (current.type != Token_New_Line) {
        string8_list_push(scratch.arena, &list, current.value);
        parser_advance(parser);
        current = parser->lexer->current_token;
      }
      String8  comment_node_value = string8_list_join(parser->arena, &list);
      AST_Node* comment_line_node = ast_node_new(parser, Node_Type_Line_Comment, comment_node_value);
      ast_node_add_child(parser, program, comment_line_node);
      parser_advance(parser);

    } else if (parser->lexer->current_token.type == Token_Comment_Block_Start) {
      Token current = parser->lexer->current_token;
      parser_advance(parser);
      String8_List list = string8_list_new(scratch.arena, current.value);
      while (current.type != Token_Comment_Block_End) {
        string8_list_push(scratch.arena, &list, current.value);
        parser_advance(parser);
        current = parser->lexer->current_token;
      }
      String8  comment_node_value = string8_list_join(parser->arena, &list);
      AST_Node* comment_line_node = ast_node_new(parser, Node_Type_Multi_Line_Comment, comment_node_value);
      ast_node_add_child(parser, program, comment_line_node);
      parser_advance(parser);

    } else if (parser->lexer->current_token.type == Token_Preprocessor_Hash) {
      parser_advance(parser);
      Token preprocessor_type = parser->lexer->current_token;
      parser_skip_whitespace(parser);
      
      if (string8_equal(preprocessor_type.value, Str8("include"))) {
        parser_advance(parser);
        parser_skip_whitespace(parser);
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
            AST_Node* node = ast_node_new(parser, Node_Type_Preprocessor_Include, header_name);
            ast_node_add_child(parser, program, node);
            parser_advance(parser);
          } break;
          case Token_String_Literal: {
            parser_advance(parser);
            Token header_name = parser->lexer->current_token;
            AST_Node* node = ast_node_from_token(parser, Node_Type_Preprocessor_Include, header_name);
            ast_node_add_child(parser, program, node);
            parser_advance(parser);
          } break;
          default: {
            // TODO(fz): Error unknown include type
          } break;
        }
      } else if (string8_equal(preprocessor_type.value, Str8("define"))) {
        // TODO(Fz): Still unhandled
        parser_advance(parser);
      } else {
        // TODO(fz): Error unhandled preprocessor type
        parser_advance(parser);
      }

    } else {
      // TODO(fz): Unhandled 
      parser_advance(parser);
    }

    scratch_end(&scratch);
  }
  return program;
}

void parser_skip_whitespace(Parser* parser) {
  while (parser->lexer->current_token.type == Token_Spaces ||
         parser->lexer->current_token.type == Token_Tabs ||
         parser->lexer->current_token.type == Token_New_Line ||
         parser->lexer->current_token.type == Token_Comment_Line ||
         parser->lexer->current_token.type == Token_Comment_Block_Start) {
    // TODO(fz): Add spaces to AST
    parser_advance(parser);
  }
}

b32 parser_expect_token(Parser* parser, Token_Type expected) {
  if (parser->lexer->current_token.type != expected) {
    return false;
  }
  parser_advance(parser);
  return true;
}

b32 parser_is_type_keyword(Token_Type type) {
  return (type == Token_Keyword_Int ||
          type == Token_Keyword_Float ||
          type == Token_Keyword_Double ||
          type == Token_Keyword_Char ||
          type == Token_Keyword_Void ||
          type == Token_Keyword_Unsigned ||
          type == Token_Keyword_Signed);
}

void parser_advance(Parser* parser) {
  lexer_get_next_token(parser->lexer);
}

///////////////
// AST

AST_Node* ast_node_new(Parser* parser, AST_Node_Type type, String8 value) {
  AST_Node* node       = ArenaPush(parser->arena, AST_Node, 1);
  node->ast_type       = type;
  node->token_type     = Token_Unknown;
  node->value          = (value.size > 0) ? value : Str8("");
  node->children       = ArenaPush(parser->arena, AST_Node*, AST_NODE_MAX_CHILDREN);
  node->children_count = 0;
  return node;
}

AST_Node* ast_node_from_token(Parser* parser, AST_Node_Type type, Token token) {
  AST_Node* node       = ArenaPush(parser->arena, AST_Node, 1);
  node->ast_type       = type;
  node->token_type     = token.type;
  node->value          = (token.value.size > 0) ? token.value : Str8("");
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
  if (node->ast_type == Node_Type_Whitespace && !print_whitespace) return;
  if ((node->ast_type == Node_Type_Line_Comment || node->ast_type == Node_Type_Multi_Line_Comment) && !print_comments) return;

  Terminal_Color color = Terminal_Color_Default;
  switch (node->ast_type) {
    case Node_Type_Whitespace: {
      color = Terminal_Color_Gray;
    } break;

    case Node_Type_Line_Comment:
    case Node_Type_Multi_Line_Comment: {
      color = Terminal_Color_Yellow;
    } break;

    case Node_Type_Preprocessor_Define:
    case Node_Type_Preprocessor_Include: {
      color = Terminal_Color_Magenta;
    } break;
  }

  // Print indentation
  for (u32 i = 0; i < indent; ++i) {
    printf("  ");
  }
  
  // Print node type and value
  printf_color(color, "%s", ast_node_types[node->ast_type]);
  if (node->value.size > 0) {
    if (!(node->ast_type && Node_Type_Whitespace ||
          node->ast_type && Node_Type_Line_Comment ||
          node->ast_type && Node_Type_Multi_Line_Comment)) {
      printf_color(color, ": \"%.*s\"", (s32)node->value.size, node->value.str);
    }
  }
  printf("\n");
  
  // Print children
  for (u32 i = 0; i < node->children_count; ++i) {
    ast_print_node(node->children[i], indent + 1, print_whitespace, print_comments);
  }
}