#ifndef PARSER_H
#define PARSER_H

///////////////
// AST

static const char8* ast_node_types [] = {
  // Root
  "AST_Node_Program",
  "AST_Node_Declaration",

  // Expressions
  "AST_Node_BinaryOp",
  "AST_Node_UnaryOp",
  "AST_Node_Assignment",
  "AST_Node_Conditional",
  "AST_Node_Call",
  "AST_Node_Cast",
  "AST_Node_Literal",
  "AST_Node_Identifier",

  // Whitespace / Comments
  "AST_Node_Space",
  "AST_Node_Tab",
  "AST_Node_NewLine",
  "AST_Node_CommentLine",
  "AST_Node_CommentBlock",

  // Preprocessor
  "AST_Node_PreprocessorIncludeSystem",
  "AST_Node_PreprocessorIncludeLocal",

  // Fallback
  "AST_Node_Unknown",
};

typedef enum AST_Node_Type {
  // Fallback
  AST_Node_Unknown = 0,

  // Root
  AST_Node_Program,
  AST_Node_Declaration,

  // Expressions
  AST_Node_Binary_Op,
  AST_Node_Unary_Op,
  AST_Node_Assignment,
  AST_Node_Conditional,    // ternary ?: 
  AST_Node_Call,           // function call
  AST_Node_Cast,
  AST_Node_Literal,
  AST_Node_Identifier,

  // Whitespace / Comments
  AST_Node_Space,
  AST_Node_Tab,
  AST_Node_New_Line,
  AST_Node_Comment_Line,
  AST_Node_Comment_Block,

  // Preprocessor
  AST_Node_Preprocessor_Include_System,
  AST_Node_Preprocessor_Include_Local,
  AST_Node_Preprocessor_Define,
  AST_Node_Preprocessor_Pragma,

} AST_Node_Type;

typedef enum AST_Binary_Op {
  Binary_Op_Add,           // +
  Binary_Op_Sub,           // -
  Binary_Op_Mul,           // *
  Binary_Op_Div,           // /
  Binary_Op_Mod,           // %

  Binary_Op_Assign,         // =
  Binary_Op_Add_Assign,     // +=
  Binary_Op_Sub_Assign,     // -=
  Binary_Op_Mul_Assign,     // *=
  Binary_Op_Div_Assign,     // /=
  Binary_Op_Mod_Assign,     // %=

  Binary_Op_Equal,          // ==
  Binary_Op_Not_Equal,      // !=
  Binary_Op_Less,           // <
  Binary_Op_Less_Equal,     // <=
  Binary_Op_Greater,        // >
  Binary_Op_Greater_Equal,  // >=

  Binary_Op_Logical_And,    // &&
  Binary_Op_Logical_Or,     // ||

  Binary_Op_Bit_And,        // &
  Binary_Op_Bit_Or,         // |
  Binary_Op_Bit_Xor,        // ^
  Binary_Op_Left_Shift,     // <<
  Binary_Op_Right_Shift,    // >>

  Binary_Op_Comma,           // ,
} AST_Binary_Op;

typedef enum AST_Unary_Op {
  Unary_Op_Plus,     // +
  Unary_Op_Minus,    // -
  Unary_Op_PreInc,   // ++a
  Unary_Op_PreDec,   // --a
  Unary_Op_PostInc,  // a++
  Unary_Op_PostDec,  // a--
  Unary_Op_Deref,    // *a
  Unary_Op_Address,  // &a
  Unary_Op_Not,      // !
  Unary_Op_BitNot,   // ~
  Unary_Op_Cast,     // (type)
} AST_Unary_Op;

typedef enum AST_Literal_Type {
  Literal_Int,
  Literal_Float,
  Literal_Char,
  Literal_String,
} AST_Literal_Type;

typedef struct AST_Node {
  Token_Array tokens;

#if DEBUG
  // This holds the string value of all tokens together. 
  // Do not use as source of truth. It's for ease of inspection.
  String8 value;
#endif

  AST_Node_Type ast_type;
  union {
    AST_Binary_Op binary_op;
    AST_Unary_Op unary_op;
    AST_Literal_Type literal_type;
  };

  struct AST_Node** children;
  u32 children_count;
} AST_Node;
#define AST_NODE_MAX_CHILDREN 1024

///////////////
// Parser

typedef struct Parser_Error {
  String8 message;
  b32 has_error;
  u32 line;
  u32 column;
} Parser_Error;

typedef struct Parser {
  Arena* arena;
  Lexer* lexer;
  Parser_Error error;
  AST_Node* root;
} Parser;

void      parser_init(Parser* parser, Lexer* lexer);
AST_Node* parser_parse_file(Parser* parser);

void parser_advance(Parser* parser);

b32 parser_expect_token(Parser* parser, Token_Type expected);
b32 parser_is_token_datatype(Parser* parser);

// Parse functions
AST_Node* parser_parse_expression(Parser* parser);
b32       parser_parse_whitespace(Parser* parser, AST_Node* parent);
b32       parser_parse_comment_line(Parser* parser);
b32       parser_parse_comment_block(Parser* parser);
b32       parser_parse_preprocessor_directives(Parser* parser);
b32       parser_parse_typedef(Parser* parser);
b32       parser_parse_function_definition(Parser* parser);
b32       parser_parse_declaration(Parser* parser);

AST_Node* ast_node_new(Parser* parser, Token_Array token_array, AST_Node_Type node_type, u32 variant);
void      ast_node_add_child(Parser* parser, AST_Node* parent, AST_Node* child);

void ast_print(AST_Node* root, b32 print_whitespace, b32 print_comments);
void ast_print_node(AST_Node* node, u32 indent, b32 print_whitespace, b32 print_comments);

#endif // PARSER_H