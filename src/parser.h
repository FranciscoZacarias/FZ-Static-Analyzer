#ifndef PARSER_H
#define PARSER_H

///////////////
// AST

static const char8* ast_node_types [] = {
  "Node_Type_Unknown",

  "Node_Type_Program",
  "Node_Type_Function_Declaration",
  "Node_Type_Function_Implementation",
  "Node_Type_Variable_Declaration",
  "Node_Type_Return_Statement",
  "Node_Type_Expression",
  "Node_Type_Parameter",
  "Node_Type_Binary_Operation",
  "Node_Type_Identifier",
  "Node_Type_Number",
  "Node_Type_Block_Statement",
  "Node_Type_If_Statement",
  "Node_Type_For_Statement",
  "Node_Type_While_Statement",
  "Node_Type_Assignment",
  "Node_Type_Function_Call",
  "Node_Type_Member_Access",
  "Node_Type_Array_Access",
  "Node_Type_Unary_Operation",
  "Node_Type_Type",
  "Node_Type_Parameter_List",
  "Node_Type_Struct_Declaration",
  "Node_Type_Field_Declaration",
  "Node_Type_Typedef",
  "Node_Type_String_Literal",
  "Node_Type_Character_Literal",

  // Preprocessor
  "Node_Type_Preprocessor_System_Include",
  "Node_Type_Preprocessor_Local_Include",
  "Node_Type_Preprocessor_Define",
  "Node_Type_Preprocessor_Pragma",

  "Node_Type_Break_Statement",
  "Node_Type_Continue_Statement",
  "Node_Type_Switch_Statement",
  "Node_Type_Case_Statement",
  "Node_Type_Default_Statement",

  // Whitespace
  "Node_Type_Space",
  "Node_Type_Tab",
  "Node_Type_New_Line",

  // Comments
  "Node_Type_Line_Comment",
  "Node_Type_Multi_Line_Comment",
};

typedef enum AST_Node_Type {
  Node_Type_Unknown = 0,

  Node_Type_Program,
  Node_Type_Function_Declaration,
  Node_Type_Function_Implementation,
  Node_Type_Variable_Declaration,
  Node_Type_Return_Statement,
  Node_Type_Expression,
  Node_Type_Parameter,
  Node_Type_Binary_Operation,
  Node_Type_Identifier,
  Node_Type_Number,
  Node_Type_Block_Statement,
  Node_Type_If_Statement,
  Node_Type_For_Statement,
  Node_Type_While_Statement,
  Node_Type_Assignment,
  Node_Type_Function_Call,
  Node_Type_Member_Access,
  Node_Type_Array_Access,
  Node_Type_Unary_Operation,
  Node_Type_Type,
  Node_Type_Parameter_List,
  Node_Type_Struct_Declaration,
  Node_Type_Field_Declaration,
  Node_Type_Typedef,
  Node_Type_String_Literal,
  Node_Type_Character_Literal,

  // Preprocessor
  Node_Type_Preprocessor_System_Include,
  Node_Type_Preprocessor_Local_Include,
  Node_Type_Preprocessor_Define,
  Node_Type_Preprocessor_Pragma,

  Node_Type_Break_Statement,
  Node_Type_Continue_Statement,
  Node_Type_Switch_Statement,
  Node_Type_Case_Statement,
  Node_Type_Default_Statement,

  // Whitespace
  Node_Type_Space,
  Node_Type_Tab,
  Node_Type_New_Line,

  // Comments
  Node_Type_Line_Comment,
  Node_Type_Multi_Line_Comment,
} AST_Node_Type;

typedef struct AST_Node {
  AST_Node_Type ast_type;
  Token_Type token_type;
  String8 value;
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

void parser_skip_whitespace(Parser* parser, AST_Node* parent);
b32  parser_expect_token(Parser* parser, Token_Type expected);
void parser_advance(Parser* parser);

AST_Node* ast_node_new(Parser* parser, AST_Node_Type type, String8 value);
AST_Node* ast_node_from_token(Parser* parser, AST_Node_Type type, Token token);
void      ast_node_add_child(Parser* parser, AST_Node* parent, AST_Node* child);

void ast_print(AST_Node* root, b32 print_whitespace, b32 print_comments);
void ast_print_node(AST_Node* node, u32 indent, b32 print_whitespace, b32 print_comments);

#endif // PARSER_H