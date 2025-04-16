#ifndef PARSER_H
#define PARSER_H

typedef enum Token_Type {
    Token_Data_Type,         // data types
    Token_Identifier,        // variable or function name
    Token_Number,            // 123

    Token_Semicolon,         // ;
    Token_Left_Parenthesis,  // (
    Token_Right_Parenthesis, // )
    Token_Left_Curly_Brace,  // {
    Token_Right_Curly_Brace, // }
    Token_Assign,            // =
    Token_Return,            // return
    
    Token_Plus,     // +
    Token_Minus,    // -
    Token_Multiply, // *
    Token_Division, // /
    Token_Module,   // %

    Token_End_Of_File,

    Token_Error
} Token_Type;

typedef struct Token{
    Token_Type type;
    String8 value;
} Token;

///////////////
// Lexer

typedef struct Workspace {
    String8 workspace_path;
    File_Data* files;
    u32 total_files;
} Workspace;

typedef struct Lexer {
    Workspace workspace;
    Token current_token;
    u32 y;
    u32 x;
} Lexer;

void lexer_init(Lexer* lexer, String8 workspace_path); /* Initializes the lexer with workspace path */

void lexer_eat_whitespace(Lexer* lexer); /* Ignores any kinds of spaces */
void lexer_get_next_token(Lexer* lexer); /*  */
void lexer_process_token(Lexer* lexer);  /*  */

///////////////
// Parser
typedef struct Parser {
    Lexer* lexer;
    Arena* arena;
} Parser;

void parser_init(Parser* parser, Lexer* lexer);

///////////////
// AST
typedef enum AST_Node_Type {
    Node_Type_Function_Declaration,
    Node_Type_Function_Implementation,
} AST_Node_Type;

typedef struct AST_Node {
    AST_Node_Type type;
    String8 value;
    struct AST_Node* left;
    struct AST_Node* right;
} AST_Node;

AST_Node* ast_node_new(Parser* parser, AST_Node_Type type, String8 value);

#endif // PARSER_H