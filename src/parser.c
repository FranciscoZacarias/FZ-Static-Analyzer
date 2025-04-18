
///////////////
// Lexer
void lexer_init(Lexer* lexer, String8 workspace_path) {
  lexer->workspace.workspace_path = workspace_path;

  lexer->current_token = (Token){ Token_Error, Str8("") };
  lexer->x = 0;
  lexer->y = 0;
}

///////////////
// Parser
void parser_init(Parser* parser, Lexer* lexer) {
  parser->arena = arena_init();
  parser->lexer = lexer;
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