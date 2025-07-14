///////////////
// Parser
void parser_init(Parser* parser, Lexer* lexer) {
  MemoryZeroStruct(parser);

  parser->arena = arena_init();
  parser->lexer = lexer;
  parser->root  = ast_node_new(parser->arena, 0, 0, AST_Node_Program);

  lexer_get_next_token(lexer);
}

AST_Node* parser_parse_file(Parser* parser) {

  while (parser->lexer->current_token.type != Token_End_Of_File) {
    // Top level constructs. Order matters.
    if (0) {
    } else if (parser_parse_preprocessor_directives(parser)) {
    } else if (parser_parse_typedef(parser)) {
    } else if (parser_parse_function_definition(parser)) {
    } else if (parser_parse_declaration(parser)) {
    } else {
      parser_advance(parser, parser->root);
    }
  }
  return parser->root;
}

b32 parser_parse_whitespace(Parser* parser, AST_Node* parent) {
  b32 result = false;
  Token first_token = parser->lexer->current_token;
  if (first_token.type == Token_Space || first_token.type == Token_Tab || first_token.type == Token_New_Line) {
    AST_Node_Type node_type = (first_token.type == Token_Space) ? AST_Node_Space :
                              (first_token.type == Token_Tab)   ? AST_Node_Tab : AST_Node_New_Line;
    u32 start_offset = first_token.start_offset;
    u32 end_offset   = first_token.end_offset;
  
    // Consume consecutive tokens of same type
    while (parser->lexer->current_token.type == first_token.type) {
      end_offset = parser->lexer->current_token.end_offset;
      lexer_get_next_token(parser->lexer);
    }
    AST_Node* node = ast_node_new(parser->arena, start_offset, end_offset, node_type);
    ast_node_add_child(parser->arena, parent, node);
    result = true;
  }

  return result;
}

void parser_advance(Parser* parser, AST_Node* parent) {
  if (0) {
  } else if (parser_parse_whitespace(parser, parent)) {
  } else if (parser_parse_comment_line(parser, parent)) {
  } else if (parser_parse_comment_block(parser, parent)) {
  } else {
    lexer_get_next_token(parser->lexer);
  }
}

Token parser_advance_semantic(Parser* parser) {
  return (Token){0};
}

Token parser_peek_semantic(Parser* parser) {
  return (Token){0};
}

b32 parser_parse_comment_line(Parser* parser, AST_Node* parent) {
  b32 result = false;
  if (parser->lexer->current_token.type == Token_Comment_Line) {
    lexer_get_next_token(parser->lexer);
    Token current_token = parser->lexer->current_token;
    u32 start_offset = parser->lexer->current_token.start_offset;
    while (current_token.type != Token_New_Line) {
      lexer_get_next_token(parser->lexer);
      current_token = parser->lexer->current_token;
    }
    u32 end_offset = parser->lexer->current_token.end_offset;
    ast_node_add_child(parser->arena, parent, ast_node_new(parser->arena, start_offset, end_offset, AST_Node_Comment_Line));
    result = true;
  }
  return result;
}

b32 parser_parse_comment_block(Parser* parser, AST_Node* parent) {
  b32 result = false;
  if (parser->lexer->current_token.type == Token_Comment_Block_Start) {
    lexer_get_next_token(parser ->lexer);
    Token current_token = parser->lexer->current_token;
    u32 start_offset = parser->lexer->current_token.start_offset;
    while (current_token.type != Token_Comment_Block_End) {
      lexer_get_next_token(parser->lexer);
      current_token = parser->lexer->current_token;
    }
    u32 end_offset = parser->lexer->current_token.end_offset - 2; // We don't want the */ to be in in the offset range
    ast_node_add_child(parser->arena, parent, ast_node_new(parser->arena, start_offset, end_offset, AST_Node_Comment_Block));
    result = true;
  }
  return result;
}

b32 parser_parse_preprocessor_directives(Parser* parser) {
  b32 result = false;
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

  // Try variable declaration first
  if (parser_parse_variable_declaration(parser)) {
    result = true;
  }
 
  // TODO: Add other declaration types here
  // if (parser_parse_struct_declaration(parser)) return true;
  // if (parser_parse_union_declaration(parser)) return true;
  // if (parser_parse_enum_declaration(parser)) return true;
 
  return result;
}

b32 parser_parse_variable_declaration(Parser* parser) {
  b32 result = false;
  return result;
}


AST_Node* parser_parse_expression(Parser* parser) {
  AST_Node* result = NULL;
  return result;
}

///////////////
// AST

// TODO(fz): Change arg parser for Arena*
AST_Node* ast_node_new(Arena* arena, u32 start_offset, u32 end_offset, AST_Node_Type type) {
  AST_Node* node       = arena_push(arena, sizeof(AST_Node));
  node->start_offset   = start_offset;
  node->end_offset     = end_offset;
  node->type           = type;
  node->children       = 0;
  node->children_count = 0;
  return node;
}

void ast_node_add_child(Arena* arena, AST_Node* parent, AST_Node* child) {
  if (parent->children_count == 0) {
    parent->children = ArenaPush(arena, AST_Node*, 4);
  }
  if (parent->children_count > 0 && (parent->children_count & (parent->children_count - 1)) == 0) {
    u32 new_capacity = parent->children_count * 2;
    // NOTE(fz): If we do a lot of reallocations, we could be filling the arena with unused memory
    AST_Node** new_children = ArenaPush(arena, AST_Node*, new_capacity);
    for (u32 i = 0; i < parent->children_count; i += 1) {
      new_children[i] = parent->children[i];
    }
    parent->children = new_children;
  }
  parent->children[parent->children_count++] = child;
}

AST_Node* ast_make_binary(Arena* arena, AST_Node* parent, AST_Node* left, AST_Node* right) {
  ast_node_add_child(arena, parent, left);
  ast_node_add_child(arena, parent, right);
  return parent;
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
  printf_color(color, "{.type=%s, .size=%d}: %.*s", ast_node_types[node->type], size, size, parser->lexer->file.data.str + node->start_offset);
  printf("\n");
  
  for (u32 i = 0; i < node->children_count; i += 1) {
    parser_print_ast_node(parser, node->children[i], indent + 1, print_whitespace, print_comments);
  }
}