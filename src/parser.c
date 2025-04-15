
///////////////
// AST

AST_Node* ast_node_new(Parser* parser, AST_Node_Type type, String value) {
    Assert(parser->arena.reserved > 0);
    AST_Node* node = ArenaPush(parser->arena, AST_Node, 1);
    node->type  = type;
    node->value = (value.size > 0) ? value : StringLiteral("");
    node->left  = NULL;
    node->right = NULL;
    return node;
}

///////////////
// Lexer
void lexer_init(Lexer* lexer, String project_path) {
    lexer->project_path  = project_path;
    lexer->current_token = (Token){ Token_Error, StringLiteral("") };
    lexer->x = 0;
    lexer->y = 0;
}

///////////////
// Parser
void parser_init(Parser* parser, Lexer* lexer) {
    parser->arena = arena_init();
    parser->lexer = lexer;
}


