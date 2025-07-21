
///////////////
// Parser
internal AST_Node* parse_ast(Parser* parser, Token_Array tokens) {
#ifndef DEBUG
  MemoryZeroStruct(parser);
#endif

  parser->arena       = arena_init();
  parser->nodes_arena = arena_init();
  parser->root        = node_new(parser->nodes_arena, 0, 0, AST_Node_Program);

  parser->tokens = tokens;
  parser->index  = 0;

  parser->errors       = ArenaPush(parser->arena, Parser_Error, PARSER_ERROR_CAPACITY);
  parser->errors_cap   = PARSER_ERROR_CAPACITY;
  parser->errors_count = 0;

  Token* current = current_token(parser);
  while (current != NULL && current->type != Token_End_Of_File) {
    AST_Node* top_level_item = get_top_level_construct(parser);
    if (top_level_item) {
      node_add_child(parser->root, top_level_item);
    } else if (is_token_trivia(*current)) {
      node_add_child(parser->root, node_new(parser->nodes_arena, current->start_offset, current->end_offset, node_type_from_trivia_token(current->type)));
    } else {
      advance_token_skip_trivia(parser, parser->root); // Add top level trivia to tree
    }
    current = advance_token_skip_trivia(parser, parser->root);
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
    return node_new(parser->nodes_arena, token->start_offset, token->end_offset, AST_Node_EmptyDecl);
  }
  
  // Preprocessor
  if (token->type == Token_Preprocessor_Hash) {
    return parse_preprocessor(parser);
  }

  return NULL;
}

internal Token* peek_token(Parser* parser, u64 offset) {
  Token* result = NULL;
  u64 i = parser->index + offset;
  if (i < parser->tokens.count) {
    result = &parser->tokens.tokens[i];
  }

  return result;
}

internal Token* advance_token(Parser* parser) {
  Token* result = NULL;
  if (parser->index < parser->tokens.count) {
    parser->index += 1;
    result = current_token(parser);
  }
  return result;
}

internal Token* advance_token_skip_trivia(Parser* parser, AST_Node* parent) {
  Token* result = advance_token(parser);
  while (is_token_trivia(*result)) {
    node_add_child(parent, node_new(parser->nodes_arena, result->start_offset, result->end_offset, node_type_from_trivia_token(result->type)));
    if (result->type == Token_End_Of_File) {
      break;
    }
    advance_token(parser);
    result = current_token(parser);
  }
  return result;
}

internal b32 is_token_trivia(Token token) {
  b32 result = false;
  Token_Type type = token.type;
  if (type == Token_Space         ||
      type == Token_Tab           ||
      type == Token_New_Line      ||
      type == Token_Comment_Line  ||
      type == Token_Comment_Block ||
      type == Token_End_Of_File) {
    result = true;
  }
  return result;
}

internal AST_Node_Type node_type_from_trivia_token(Token_Type type) {
  AST_Node_Type result = AST_Node_Unknown;
  switch (type) {
    case Token_Space:         { result = AST_Node_Space; }         break;
    case Token_Tab:           { result = AST_Node_Tab; }           break;
    case Token_New_Line:      { result = AST_Node_New_Line; }      break;
    case Token_Comment_Line:  { result = AST_Node_Comment_Line; }  break;
    case Token_Comment_Block: { result = AST_Node_Comment_Block; } break;
  }
  return result;
}

internal Token* assert_token(Parser* parser, Token_Type type) {
  Token* t = peek_token(parser, 0);
  if (!t || t->type != type) {
    ERROR_MESSAGE_AND_EXIT("Expected token %s but got %s.", ast_node_types[type], ast_node_types[t->type]);
    exit(1);
  }
  return t;
}

/*
  Program:
    - Declaration:
      - < // not in tree
      - [start]stdio
      - .
      - h[end]
      - > // not in tree
*/
internal AST_Node* parse_preprocessor(Parser* parser) {
  Token* hash_token = current_token(parser);
  Assert(hash_token->type == Token_Preprocessor_Hash);

  AST_Node* result = NULL;
  Token_Type preprocessor_type = Token_Unknown;
  Token* preprocessor_token    = advance_token_skip_trivia(parser, parser->root);

  if (0) {
  } else if (string8_equal(preprocessor_token->value, Str8("include"))) {
    Token* next = advance_token_skip_trivia(parser, parser->root);
    if (next->type == Token_Less) {
      Token* current = advance_token_skip_trivia(parser, parser->root);
      u32 start = current->start_offset;
      while (current->type != Token_Greater) {
        current = advance_token_skip_trivia(parser, parser->root);
        if (current->type == Token_End_Of_File) {
          ERROR_MESSAGE_AND_EXIT("Unexpected end of file");
        }
      }
      // NOTE(fz): current->end_offset - 1 to skip '>'
      result = node_new(parser->nodes_arena, start, current->end_offset - 1, AST_Node_Preprocessor_Include_System);
      advance_token(parser); // skip- Token_Greater
    } else if (next->type == Token_String_Literal) {
      result = node_new(parser->nodes_arena, preprocessor_token->start_offset, preprocessor_token->end_offset, AST_Node_Preprocessor_Include_Local);
    } else {
      ERROR_MESSAGE_AND_EXIT("Unhandled preprocessor: %s", token_type_names[next->type]);
    }
  } else if (string8_equal(preprocessor_token->value, Str8("define"))) {
  }

  return result;
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
  node->arena          = arena;
  node->children       = NULL;
  node->children_count = 0;
  node->children_max   = 0;
  node->type           = type;
  node->start_offset   = start_offset;
  node->end_offset     = end_offset;
  return node;
}

internal void node_add_child(AST_Node* parent, AST_Node* child) {
  Assert(parent->arena == child->arena);
  if (parent->children_count == parent->children_max) {
    u32 new_capacity = (parent->children_max == 0) ? 2 : parent->children_max * 2;
    AST_Node** new_children = ArenaPush(parent->arena, AST_Node*, new_capacity);
    MemoryCopy(new_children, parent->children, parent->children_count * sizeof(AST_Node*));
    parent->children = new_children;
    parent->children_max = new_capacity;
  }
  parent->children[parent->children_count] = child;
  parent->children_count += 1;
}

internal AST_Node* make_binary(AST_Node* parent, AST_Node* left, AST_Node* right) {
  node_add_child(parent, left);
  node_add_child(parent, right);
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
  printf_color(color, "{.type=%s, }: %.*s", ast_node_types[node->type], size, lexer->file.data.str + node->start_offset);
  printf("\n");
  
  for (u32 i = 0; i < node->children_count; i += 1) {
    print_ast_node(parser, lexer, node->children[i], indent + 1, print_whitespace, print_comments);
  }
}