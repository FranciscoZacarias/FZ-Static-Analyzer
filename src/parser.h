#ifndef PARSER_H
#define PARSER_H

static const char8* ast_node_types [] = {
  "AST_Node_Unknown",
  "AST_Node_Program",
  "AST_Node_Declaration",
  "AST_Node_EmptyDecl",
  "AST_Node_Data_Type",
  "AST_Node_Expression_End",
  
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
  AST_Node_EmptyDecl,
  AST_Node_Data_Type,
  AST_Node_Expression_End,
  
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
  Arena* arena;
  struct AST_Node** children;
  u32 children_count;
  u32 children_max;
  AST_Node_Type type;
  u32 start_offset;
  u32 end_offset;
} AST_Node;

///////////////
// Parser

typedef struct Parser_Error {
  String8 message;
  u32 start_offset;
  u32 end_offset;
} Parser_Error;

typedef struct Parser {
  Arena* arena;
  Arena* nodes_arena;
  AST_Node* root;

#if DEBUG
  File_Data* file;
#endif

  Token_Array tokens;
  u64 index;
  
  Parser_Error* errors;
  u32 errors_count;
  u32 errors_cap;
} Parser;
#define PARSER_ERROR_CAPACITY 32

internal AST_Node* parse_ast(Parser* parser, Token_Array tokens);
internal AST_Node* get_top_level_construct(Parser* parser);

// Parser token modifying
internal Token* peek_token(Parser* parser, u64 offset);
internal Token* advance_token(Parser* parser);
internal Token* advance_token_skip_trivia(Parser* parser, AST_Node* parent);
internal Token* assert_token(Parser* parser, Token_Type type);

// Parser help
internal AST_Node* parse_preprocessor(Parser* parser);
internal void      parser_emit_error(Parser* parser, u32 start_offset, u32 end_offset, String8 message);

// Token help
internal b32 is_token_trivia(Token token);
internal AST_Node_Type node_type_from_trivia_token(Token_Type type);

// AST
internal AST_Node* node_new(Arena* arena, u32 start_offset, u32 end_offset, AST_Node_Type type);
internal void      node_add_child(AST_Node* parent, AST_Node* child);
internal AST_Node* make_binary(AST_Node* parent, AST_Node* left, AST_Node* right);

internal void print_ast(Parser* parser, Lexer* lexer, b32 print_whitespace, b32 print_comments);
internal void print_ast_node(Parser* parser, Lexer* lexer, AST_Node* node, u32 indent, b32 print_whitespace, b32 print_comments);

#endif // PARSER_H