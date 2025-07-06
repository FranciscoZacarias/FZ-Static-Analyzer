///////////////
// Parser
void parser_init(Parser* parser, Lexer* lexer) {
  MemoryZeroStruct(parser);
  parser->arena = arena_init();
  parser->lexer = lexer;
  lexer_get_next_token(lexer);
}

AST_Node* parser_parse_file(Parser* parser) {
  Arena_Temp scratch = scratch_begin(0,0);
  AST_Node* program = ast_node_new(parser, Node_Type_Program, Str8(""));

  while (parser->lexer->current_token.type != Token_End_Of_File) {
    if (parser->lexer->current_token.type == Token_Spaces ||
        parser->lexer->current_token.type == Token_Tabs ||
        parser->lexer->current_token.type == Token_New_Line) {
      AST_Node* whitespace_node = ast_node_new(parser, Node_Type_Whitespace, parser->lexer->current_token.value);
      ast_node_add_child(parser, program, whitespace_node);
      parser_advance(parser);
    } else if (parser->lexer->current_token.type == Token_Comment_Line) {
      Token current = parser->lexer->current_token;
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
    
      parser_advance(parser);
    } else {
      AST_Node* global_declaration = parser_get_global_declaration(parser);
      if (global_declaration) {
        ast_node_add_child(parser, program, global_declaration);
      } else {
        parser_advance(parser);
      }
    }
  }
  scratch_end(&scratch);
  return program;
}

AST_Node* parser_get_global_declaration(Parser* parser) {
  parser_skip_whitespace(parser);
  
  Token current = parser->lexer->current_token;
  
  // Handle preprocessor directives
  if (current.type == Token_Preprocessor_Include) {
    return parser_get_preprocessor_include(parser);
  }
  if (current.type == Token_Preprocessor_Define) {
    return parser_get_preprocessor_define(parser);
  }
  
  // Handle typedef
  if (current.type == Token_Keyword_Typedef) {
    return parser_get_typedef(parser);
  }
  
  // Parse type (int, float, char, etc.)
  if (!parser_is_type_keyword(current.type)) {
    return NULL; // Not a valid declaration
  }
  
  String8 type_name = current.value;
  parser_advance(parser);
  parser_skip_whitespace(parser);
  
  // Parse identifier name
  if (parser->lexer->current_token.type != Token_Identifier) {
    return NULL; // Error: expected identifier
  }
  
  String8 name = parser->lexer->current_token.value;
  parser_advance(parser);
  parser_skip_whitespace(parser);
  
  // Determine if it's a function or variable
  if (parser->lexer->current_token.type == Token_Left_Parenthesis) {
    // Function declaration: int add(...)
    return parser_get_function(parser, type_name, name);
  } else {
    // Variable declaration: int x = 5;
    return parser_get_variable_declaration(parser, type_name, name);
  }
}

AST_Node* parser_get_function(Parser* parser, String8 type, String8 name) {
  return NULL;
}

AST_Node* parser_get_variable_declaration(Parser* parser, String8 type, String8 name) {
  return NULL;
}

AST_Node* parser_get_preprocessor_include(Parser* parser) {
  parser_advance(parser); // consume '#include'
  parser_skip_whitespace(parser);
  
  // Expect string literal or angle brackets
  if (parser->lexer->current_token.type == Token_String) {
    String8 include_path = parser->lexer->current_token.value;
    parser_advance(parser);
    return ast_node_new(parser, Node_Type_Preprocessor_Include, include_path);
  } else if (parser->lexer->current_token.type == Token_Less) {
    // Handle <stdio.h> style includes
    parser_advance(parser); // consume '<'
    
    // Collect everything until '>'
    char8* start = parser->lexer->current_character;
    while (parser->lexer->current_token.type != Token_Greater && 
           parser->lexer->current_token.type != Token_End_Of_File) {
      parser_advance(parser);
    }
    char8* end = parser->lexer->current_character;
    
    if (parser->lexer->current_token.type == Token_Greater) {
      parser_advance(parser); // consume '>'
      String8 include_path = {(u64)(end - start), start};
      return ast_node_new(parser, Node_Type_Preprocessor_Include, include_path);
    }
  }

  return NULL;
}

AST_Node* parser_get_preprocessor_define(Parser* parser) {
  return NULL;
}

AST_Node* parser_get_typedef(Parser* parser) {
  return NULL;
}

void parser_skip_whitespace(Parser* parser) {
  while (parser->lexer->current_token.type == Token_Spaces ||
         parser->lexer->current_token.type == Token_Tabs ||
         parser->lexer->current_token.type == Token_New_Line ||
         parser->lexer->current_token.type == Token_Comment_Line ||
         parser->lexer->current_token.type == Token_Comment_Block_Start) {
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
  node->type           = type;
  node->value          = (value.size > 0) ? value : Str8("");
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

void ast_print(AST_Node* root) {
  printf("AST:\n");
  ast_print_node(root, 0);
}

void ast_print_node(AST_Node* node, u32 indent) {
  if (!node) return;
  
  // Print indentation
  for (u32 i = 0; i < indent; ++i) {
    printf("  ");
  }
  
  // Print node type and value
  printf("%s", ast_node_types[node->type]);
  if (node->value.size > 0) {
    if (!(node->type && Node_Type_Whitespace ||
          node->type && Node_Type_Line_Comment ||
          node->type && Node_Type_Multi_Line_Comment)) {
      printf(": \"%.*s\"", (s32)node->value.size, node->value.str);
    }
  }
  printf("\n");
  
  // Print children
  for (u32 i = 0; i < node->children_count; ++i) {
    ast_print_node(node->children[i], indent + 1);
  }
}