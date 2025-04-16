
///////////////
// Lexer
void lexer_init(Lexer* lexer, String8 workspace_path) {
    lexer->workspace.workspace_path = workspace_path;

    lexer->current_token = (Token){ Token_Error, Str8("") };
    lexer->x = 0;
    lexer->y = 0;
}

void lexer_eat_whitespace(Lexer* lexer) {
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
    Assert(parser->arena.reserved > 0);
    AST_Node* node = ArenaPush(parser->arena, AST_Node, 1);
    node->type  = type;
    node->value = (value.size > 0) ? value : Str8("");
    node->left  = NULL;
    node->right = NULL;
    return node;
}

