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

b32 parser_parse_comment_line(Parser* parser) {
  b32 result = false;
  if (parser->lexer->current_token.type == Token_Comment_Line) {
    parser_advance(parser);
    Token current_token = parser->lexer->current_token;
    u32 start_offset = parser->lexer->current_token.start_offset;
    while (current_token.type != Token_New_Line) {
      parser_advance(parser);
      current_token = parser->lexer->current_token;
    }
    u32 end_offset = parser->lexer->current_token.end_offset;
    ast_node_add_child(parser, parser->root, ast_node_new(parser, start_offset, end_offset, AST_Node_Comment_Line));
    result = true;
    parser_advance(parser);
  }
  return result;
}

b32 parser_parse_comment_block(Parser* parser) {
  b32 result = false;
  if (parser->lexer->current_token.type == Token_Comment_Block_Start) {
    parser_advance(parser);
    Token current_token = parser->lexer->current_token;
    u32 start_offset = parser->lexer->current_token.start_offset;
    while (current_token.type != Token_Comment_Block_End) {
      parser_advance(parser);
      current_token = parser->lexer->current_token;
    }
    u32 end_offset = parser->lexer->current_token.end_offset - 2; // We don't want the */ to be in in the offset range
    ast_node_add_child(parser, parser->root, ast_node_new(parser, start_offset, end_offset, AST_Node_Comment_Block));
    result = true;
    parser_advance(parser);
  }
  return result;
}

b32 parser_parse_preprocessor_directives(Parser* parser) {
  b32 result = false;
  if (parser->lexer->current_token.type == Token_Preprocessor_Hash) {
    parser_advance(parser);
    Token preprocessor_directive = parser->lexer->current_token;
    parser_advance(parser);
    parser_parse_whitespace(parser, parser->root);

    Token current_token = parser->lexer->current_token;
    u32 start_offset = current_token.start_offset;
    u32 end_offset   = current_token.end_offset;
    if (0) {
    } else if (string8_equal(preprocessor_directive.value, Str8("include"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("define"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("undef"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("ifdef"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("ifndef"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("if"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("elif"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("else"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("endif"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("error"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("warning"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("line"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8("pragma"))) {
      parser_advance(parser);
    } else if (string8_equal(preprocessor_directive.value, Str8(""))) {
      parser_advance(parser); // Empty directive: stray '#' with nothing after it
    } else {
      parser_advance(parser); // Unknown or unsupported directive
    }
  }
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

  if (!parser_is_token_datatype(parser)) {
    return false;
  }
 
  u32 start_offset = parser->lexer->current_token.start_offset;
  Token type_token = parser->lexer->current_token;
  parser_advance(parser);
  parser_parse_whitespace(parser, parser->root); // TODO(fz): Root might be wrong? Maybe not.
 
  // Handle pointer declarators
  while (parser->lexer->current_token.type == Token_Multiply) {
    parser_advance(parser);
    parser_parse_whitespace(parser, parser->root); // TODO(fz): Root might be wrong? Maybe not.
  }
 
  if (parser->lexer->current_token.type != Token_Identifier) return false;
 
  Token identifier_token = parser->lexer->current_token;
  parser_advance(parser);
  parser_parse_whitespace(parser, parser->root); // TODO(fz): Root might be wrong? Maybe not.

  u32 end_offset = identifier_token.end_offset;
 
  // Optional initializer
  AST_Node* init_expr = NULL;
  if (parser->lexer->current_token.type == Token_Assign) {
    parser_advance(parser);
    parser_parse_whitespace(parser, parser->root); // TODO(fz): Root might be wrong? Maybe not.

    // TODO(fz): This is where we parse the expression.
    // Right now we just take the full string value.
    init_expr = parser_parse_expression(parser);
  }

  if (!parser_expect_token(parser, Token_Semicolon)) {
    parser->error.has_error = true;
    parser->error.message = Str8("Expected ';' after variable declaration");
    return false;
  }
 
  end_offset = parser->lexer->current_token.end_offset;
 
  // Create AST node
  AST_Node* declaration_node = ast_node_new(parser, start_offset, end_offset, AST_Node_Declaration);
 
  // Add type as child
  AST_Node* type_node = ast_node_new(parser, type_token.start_offset, type_token.end_offset, AST_Node_Data_Type);
  ast_node_add_child(parser, declaration_node, type_node);
 
  // Add identifier as child
  AST_Node* id_node = ast_node_new(parser, identifier_token.start_offset, identifier_token.end_offset, AST_Node_Identifier);
  ast_node_add_child(parser, declaration_node, id_node);
 
  // Add initializer if present
  if (init_expr) {
    ast_node_add_child(parser, declaration_node, init_expr);
  }
 
  ast_node_add_child(parser, parser->root, declaration_node);
  return true;
}

internal AST_Node* parser_parse_expression(Parser* parser) {
  AST_Node* result = NULL;

  return result;
}

internal AST_Node_Type token_to_binary_node_type(Token_Type type) {
  switch (type) {
    case Token_Plus:            return AST_Node_Add;
    case Token_Minus:           return AST_Node_Sub;
    case Token_Multiply:        return AST_Node_Mul;
    case Token_Divide:          return AST_Node_Div;
    case Token_Modulo:          return AST_Node_Mod;
    case Token_Assign:          return AST_Node_Assign;
    case Token_Plus_Assign:     return AST_Node_Add_Assign;
    case Token_Minus_Assign:    return AST_Node_Sub_Assign;
    case Token_Multiply_Assign: return AST_Node_Mul_Assign;
    case Token_Divide_Assign:   return AST_Node_Div_Assign;
    case Token_Modulo_Assign:   return AST_Node_Mod_Assign;
    case Token_Equal:           return AST_Node_Equal;
    case Token_Not_Equal:       return AST_Node_Not_Equal;
    case Token_Less:            return AST_Node_Less;
    case Token_Less_Equal:      return AST_Node_Less_Equal;
    case Token_Greater:         return AST_Node_Greater;
    case Token_Greater_Equal:   return AST_Node_Greater_Equal;
    case Token_Logical_And:     return AST_Node_Logical_And;
    case Token_Logical_Or:      return AST_Node_Logical_Or;
    case Token_Bit_And:         return AST_Node_Bit_And;
    case Token_Bit_Or:          return AST_Node_Bit_Or;
    case Token_Bit_Xor:         return AST_Node_Bit_Xor;
    case Token_Left_Shift:      return AST_Node_Left_Shift;
    case Token_Right_Shift:     return AST_Node_Right_Shift;
    case Token_Comma:           return AST_Node_Comma;
    default:                    return AST_Node_Unknown;
  }
}

///////////////
// AST

// TODO(fz): Change arg parser for Arena*
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

AST_Node* ast_make_binary(Parser* parser, AST_Node* parent, AST_Node* left, AST_Node* right) {
  ast_node_add_child(parser, parent, left);
  ast_node_add_child(parser, parent, right);
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
  printf_color(color, "{%s, %d}: %.*s", ast_node_types[node->type], size, size, parser->lexer->file.data.str + node->start_offset);
  printf("\n");
  
  for (u32 i = 0; i < node->children_count; i += 1) {
    parser_print_ast_node(parser, node->children[i], indent + 1, print_whitespace, print_comments);
  }
}