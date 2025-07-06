#ifndef PARSER_H
#define PARSER_H

///////////////
// AST

static const char8* ast_node_types [] = {
  "Program",
  "Function_Declaration",
  "Function_Implementation",
  "Variable_Declaration",
  "Return_Statement",
  "Expression",
  "Parameter",
  "Binary_Operation",
  "Identifier",
  "Number",
  "Block_Statement",
  "If_Statement",
  "For_Statement",
  "While_Statement",
  "Assignment",
  "Function_Call",
  "Member_Access",
  "Array_Access",
  "Unary_Operation",
  "Type",
  "Parameter_List",
  "Struct_Declaration",
  "Field_Declaration",
  "Typedef",
  "String_Literal",
  "Character_Literal",
  "Preprocessor_Include",
  "Preprocessor_Define",
  "Break_Statement",
  "Continue_Statement",
  "Switch_Statement",
  "Case_Statement",
  "Default_Statement",
  "Node_Type_Whitespace",
  "Node_Type_Line_Comment",
  "Node_Type_Multi_Line_Comment",
};

typedef enum AST_Node_Type {
  Node_Type_Program = 0,
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
  Node_Type_Preprocessor_Include,
  Node_Type_Preprocessor_Define,
  Node_Type_Break_Statement,
  Node_Type_Continue_Statement,
  Node_Type_Switch_Statement,
  Node_Type_Case_Statement,
  Node_Type_Default_Statement,
  Node_Type_Whitespace,
  Node_Type_Line_Comment,
  Node_Type_Multi_Line_Comment,
} AST_Node_Type;

typedef struct AST_Node {
  AST_Node_Type type;
  String8 value;
  struct AST_Node** children;
  u32 children_count;
} AST_Node;
#define AST_NODE_MAX_CHILDREN 124

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
} Parser;

void      parser_init(Parser* parser, Lexer* lexer);
AST_Node* parser_parse_file(Parser* parser);

AST_Node* parser_get_global_declaration(Parser* parser);
AST_Node* parser_get_function(Parser* parser, String8 type, String8 name);
AST_Node* parser_get_variable_declaration(Parser* parser, String8 type, String8 name);
AST_Node* parser_get_preprocessor_include(Parser* parser);
AST_Node* parser_get_preprocessor_define(Parser* parser);
AST_Node* parser_get_typedef(Parser* parser);

void parser_skip_whitespace(Parser* parser);
b32  parser_expect_token(Parser* parser, Token_Type expected);
b32  parser_is_type_keyword(Token_Type type);
void parser_advance(Parser* parser);

AST_Node* ast_node_new(Parser* parser, AST_Node_Type type, String8 value);
void      ast_node_add_child(Parser* parser, AST_Node* parent, AST_Node* child);

void ast_print(AST_Node* root);
void ast_print_node(AST_Node* node, u32 indent);

#endif // PARSER_H