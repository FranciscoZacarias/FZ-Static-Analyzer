#ifndef PARSER_H
#define PARSER_H

static const char8* ast_node_types [] = {
  "AST_Node_Unknown",
  "AST_Node_Program",
  "AST_Node_Declaration",
  "AST_Node_Data_Type",
  
  // Binary operations
  "AST_Node_Add",
  "AST_Node_Sub",
  "AST_Node_Mul",
  "AST_Node_Div",
  "AST_Node_Mod",
  "AST_Node_Assign",
  "AST_Node_Add_Assign",
  "AST_Node_Sub_Assign",
  "AST_Node_Mul_Assign",
  "AST_Node_Div_Assign",
  "AST_Node_Mod_Assign",
  "AST_Node_Equal",
  "AST_Node_Not_Equal",
  "AST_Node_Less",
  "AST_Node_Less_Equal",
  "AST_Node_Greater",
  "AST_Node_Greater_Equal",
  "AST_Node_Logical_And",
  "AST_Node_Logical_Or",
  "AST_Node_Bit_And",
  "AST_Node_Bit_Or",
  "AST_Node_Bit_Xor",
  "AST_Node_Left_Shift",
  "AST_Node_Right_Shift",
  "AST_Node_Comma",
  
  // Unary operations
  "AST_Node_Unary_Plus",
  "AST_Node_Unary_Minus",
  "AST_Node_Pre_Inc",
  "AST_Node_Pre_Dec",
  "AST_Node_Post_Inc",
  "AST_Node_Post_Dec",
  "AST_Node_Deref",
  "AST_Node_Address",
  "AST_Node_Not",
  "AST_Node_Bit_Not",
  "AST_Node_Cast",
  
  // Literals
  "AST_Node_Literal_Int",
  "AST_Node_Literal_Float",
  "AST_Node_Literal_Char",
  "AST_Node_Literal_String",
  
  "AST_Node_Identifier",
  "AST_Node_Conditional",
  "AST_Node_Call",
  
  // Whitespace/Comments
  "AST_Node_Space",
  "AST_Node_Tab",
  "AST_Node_New_Line",
  "AST_Node_Comment_Line",
  "AST_Node_Comment_Block",
  
  // Preprocessor
  "AST_Node_Preprocessor_Include_System",
  "AST_Node_Preprocessor_Include_Local",
  "AST_Node_Preprocessor_Define",
  "AST_Node_Preprocessor_Pragma",
};

typedef enum AST_Node_Type {
  AST_Node_Unknown = 0,
  AST_Node_Program,
  AST_Node_Declaration,
  AST_Node_Data_Type,
  
  // Binary operations
  AST_Node_Add,
  AST_Node_Sub,
  AST_Node_Mul,
  AST_Node_Div,
  AST_Node_Mod,
  AST_Node_Assign,
  AST_Node_Add_Assign,
  AST_Node_Sub_Assign,
  AST_Node_Mul_Assign,
  AST_Node_Div_Assign,
  AST_Node_Mod_Assign,
  AST_Node_Equal,
  AST_Node_Not_Equal,
  AST_Node_Less,
  AST_Node_Less_Equal,
  AST_Node_Greater,
  AST_Node_Greater_Equal,
  AST_Node_Logical_And,
  AST_Node_Logical_Or,
  AST_Node_Bit_And,
  AST_Node_Bit_Or,
  AST_Node_Bit_Xor,
  AST_Node_Left_Shift,
  AST_Node_Right_Shift,
  AST_Node_Comma,
  
  // Unary operations
  AST_Node_Unary_Plus,
  AST_Node_Unary_Minus,
  AST_Node_Pre_Inc,
  AST_Node_Pre_Dec,
  AST_Node_Post_Inc,
  AST_Node_Post_Dec,
  AST_Node_Deref,
  AST_Node_Address,
  AST_Node_Not,
  AST_Node_Bit_Not,
  AST_Node_Cast,
  
  // Literals
  AST_Node_Literal_Int,
  AST_Node_Literal_Float,
  AST_Node_Literal_Char,
  AST_Node_Literal_String,
  
  AST_Node_Identifier,
  AST_Node_Conditional,
  AST_Node_Call,
  
  // Whitespace/Comments
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

typedef struct AST_Node {
  struct AST_Node** children;
  u32 children_count;
  AST_Node_Type type;
  u32 start_offset;
  u32 end_offset;
} AST_Node;

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
void      parser_advance(Parser* parser, AST_Node* parent); /* Advances tokens by 1. Parses whitespace ahead */

b32   parser_parse_declaration(Parser* parser);
b32   parser_parse_variable_declaration(Parser* parse);

void  parser_parse_whitespace(Parser* parser, AST_Node* parent);
b32   parser_parse_comment_line(Parser* parser, AST_Node* parent);
b32   parser_parse_comment_block(Parser* parser, AST_Node* parent);

// Expression
AST_Node* parser_parse_expression(Parser* parser);

// AST
AST_Node* ast_node_new(Arena* arena, u32 start_offset, u32 end_offset, AST_Node_Type type);
void      ast_node_add_child(Arena* arena, AST_Node* parent, AST_Node* child);
AST_Node* ast_make_binary(Arena* arena, AST_Node* parent, AST_Node* left, AST_Node* right);

void parser_print_ast(Parser* parser, b32 print_whitespace, b32 print_comments);
void parser_print_ast_node(Parser* parser, AST_Node* node, u32 indent, b32 print_whitespace, b32 print_comments);

#endif // PARSER_H