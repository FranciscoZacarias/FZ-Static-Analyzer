
///////////////
// Parser
internal AST_Node* parse_ast(Parser* parser, Token_Array tokens) {
  MemoryZeroStruct(parser);

  parser->arena  = arena_init();
  parser->root   = node_new(parser->arena, 0, 0, AST_Node_Program);

  parser->tokens = tokens;
  parser->index  = 0;

  parser->errors       = ArenaPush(parser->arena, Parser_Error, PARSER_ERROR_CAPACITY);
  parser->errors_cap   = PARSER_ERROR_CAPACITY;
  parser->errors_count = 0;

  while ((peek_token(parser, 0))->type != Token_End_Of_File) {
    AST_Node* top_level_item = get_top_level_construct(parser);

    if (top_level_item) {
      node_add_child(parser->arena, parser->root, top_level_item);
    } else {
      // Basic error recovery
      advance_token(parser);
    }
  }

  return parser->root;
}

internal AST_Node* get_top_level_construct(Parser* parser) {
  Token* token = peek_token(parser, 0);

  if (token->type == Token_Unknown || token->type == Token_End_Of_File) {
    return NULL;
  }

  // Empty declaration
  if (token->type == Token_Semicolon) {
    advance_token(parser);
    return node_new(parser->arena, token->start_offset, token->end_offset, AST_Node_EmptyDecl);
  }
  
  // Preprocessor
  if (token->type == Token_Preprocessor_Hash) {
    advance_token(parser);
    return parse_preprocessor(parser);
  }

  //return parse_decl_or_function(parser);
  return NULL;
}

internal Token* peek_token(Parser* parser, u64 offset) {
  u64 i = parser->index + offset;
  if (i >= parser->tokens.count) return 0;
  return &parser->tokens.tokens[i];
}

internal Token* advance_token(Parser* parser) {
  Token* result = NULL;
  if (parser->index < parser->tokens.count) {
    result = &parser->tokens.tokens[parser->index];
    parser->index += 1;
  }
  return result;
}

internal b32 match_token(Parser* parser, Token_Type type) {
  Token* t = peek_token(parser, 0);
  if (t && t->type == type) {
    advance_token(parser);
    return 1;
  }
  return 0;
}

internal Token* assert_token(Parser* parser, Token_Type type) {
  Token* t = peek_token(parser, 0);
  if (!t || t->type != type) {
    ERROR_MESSAGE_AND_EXIT("Expected token %s but got %s.", ast_node_types[type], ast_node_types[t->type]);
    exit(1);
  }
  return t;
}

internal AST_Node* parse_preprocessor(Parser* parser) {
  Token_Type preprocessor_type = Token_Unknown;
  Token* preprocessor = assert_token(parser, Token_Identifier);

  if (0) {
  } else if (string8_equal(preprocessor->value, Str8("include"))) {
    Token* next = advance_token(parser);
    if (next->type == Token_Less) {
      
    } else if (next->type == Token_String_Literal) {
      
    } else {
      ERROR_MESSAGE_AND_EXIT("Unhandled preprocessor: %s", token_type_names[next->type]);
    }

  } else if (string8_equal(preprocessor->value, Str8("define"))) {
    
  }

  return NULL;
}

internal void parser_emit_error(Parser* parser, u32 start_offset, u32 end_offset, String8 message) {
  if (parser->errors_count >= parser->errors_cap) {
    return;
  }

  Parser_Error* error = &parser->errors[parser->errors_count++];
  error->message      = string8_copy(parser->arena, message);
  error->start_offset = start_offset;
  error->end_offset   = end_offset;
}

///////////////
// AST

// TODO(fz): Change arg parser for Arena*
internal AST_Node* node_new(Arena* arena, u32 start_offset, u32 end_offset, AST_Node_Type type) {
  AST_Node* node       = arena_push(arena, sizeof(AST_Node));
  node->start_offset   = start_offset;
  node->end_offset     = end_offset;
  node->type           = type;
  node->children       = 0;
  node->children_count = 0;
  return node;
}

internal void node_add_child(Arena* arena, AST_Node* parent, AST_Node* child) {
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

internal AST_Node* make_binary(Arena* arena, AST_Node* parent, AST_Node* left, AST_Node* right) {
  node_add_child(arena, parent, left);
  node_add_child(arena, parent, right);
  return parent;
}

internal void print_ast(Parser* parser, Lexer* lexer, b32 print_whitespace, b32 print_comments) {
  print_ast_node(parser, lexer, parser->root, 0, print_whitespace, print_comments);
}

internal void print_ast_node(Parser* parser, Lexer* lexer, AST_Node* node, u32 indent, b32 print_whitespace, b32 print_comments) {
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
  printf_color(color, "{.type=%s, .size=%d}: %.*s", ast_node_types[node->type], size, size, lexer->file.data.str + node->start_offset);
  printf("\n");
  
  for (u32 i = 0; i < node->children_count; i += 1) {
    print_ast_node(parser, lexer, node->children[i], indent + 1, print_whitespace, print_comments);
  }
}