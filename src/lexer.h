#ifndef LEXER_H
#define LEXER_H

///////////////
// Token

global const char* token_type_names[] = {
  "Token_Unknown",

  // Literals
  "Token_Identifier",
  "Token_Int_Literal",
  "Token_Float_Literal",
  "Token_Hex_Literal",
  "Token_String_Literal",
  "Token_Char_Literal",

  // Keywords
  "Token_Keyword_Return",
  "Token_Keyword_If",
  "Token_Keyword_Else",
  "Token_Keyword_While",
  "Token_Keyword_For",
  "Token_Keyword_Break",
  "Token_Keyword_Continue",
  "Token_Keyword_Struct",
  "Token_Keyword_Union",
  "Token_Keyword_Enum",
  "Token_Keyword_Typedef",
  "Token_Keyword_Static",
  "Token_Keyword_Void",
  "Token_Keyword_Int",
  "Token_Keyword_Char",
  "Token_Keyword_Float",
  "Token_Keyword_Double",
  "Token_Keyword_Unsigned",
  "Token_Keyword_Signed",
  "Token_Keyword_Const",
  "Token_Keyword_Extern",
  "Token_Keyword_Switch",
  "Token_Keyword_Case",
  "Token_Keyword_Default",
  "Token_Keyword_Sizeof",
  "Token_Keyword_Inline",
  "Token_Keyword_Do",
  "Token_Keyword_Goto",
  "Token_Keyword_Restrict",
  "Token_Keyword_Volatile",
  "Token_Keyword_Register",

  // Preprocessor
  "Token_Preprocessor_Hash", // #

  // Operators
  "Token_Plus",            // +
  "Token_Plus_Assign",     // +=
  "Token_Increment",       // ++
  "Token_Minus",           // -
  "Token_Minus_Assign",    // -=
  "Token_Decrement",       // --
  "Token_Arrow",           // ->
  "Token_Multiply",        // *
  "Token_Multiply_Assign", // *=
  "Token_Divide",          // /
  "Token_Divide_Assign",   // /=
  "Token_Modulo",          // %
  "Token_Modulo_Assign",   // %=

  "Token_Assign",             // =
  "Token_Equal",              // ==
  "Token_Not",                // !
  "Token_Not_Equal",          // !=
  "Token_Less",               // <
  "Token_Less_Equal",         // <=
  "Token_Left_Shift",         // <<
  "Token_Left_Shift_Assign",  // <<=
  "Token_Greater",            // >
  "Token_Greater_Equal",      // >=
  "Token_Right_Shift",        // >>
  "Token_Right_Shift_Assign", // >>=

  "Token_Bit_And",         // &
  "Token_Bit_And_Assign",  // &=
  "Token_Logical_And",     // &&
  "Token_Bit_Or",          // |
  "Token_Bit_Or_Assign",   // |=
  "Token_Logical_Or",      // ||

  "Token_Bit_Xor",         // ^
  "Token_Bit_Xor_Assign",  // ^=
  "Token_Bit_Not",         // ~

  // Delimiters
  "Token_Semicolon",        // ;
  "Token_Comma",            // ,  
  "Token_Dot",              // .
  "Token_Colon",            // :
  "Token_Question",         // ?

  "Token_Open_Parenthesis",  // (
  "Token_Close_Parenthesis", // )
  "Token_Open_Brace",        // {
  "Token_Close_Brace",       // }
  "Token_Open_Bracket",      // [
  "Token_Close_Bracket",     // ]

  // Special
  "Token_Space",                // ' '
  "Token_Tab",                  // /t
  "Token_New_Line",              // \n, \r, or \r\n
  "Token_Comment_Line",          // //
  "Token_Comment_Block",   // /*

  "Token_End_Of_File",

  "Token_Count", // Keep at the end of the enum
};

typedef enum Token_Type {
  Token_Unknown = 0,

  // Literals
  Token_Identifier,
  Token_Int_Literal,
  Token_Float_Literal,
  Token_Hex_Literal,
  Token_String_Literal,
  Token_Char_Literal,

  // Keywords
  Token_Keyword_Return,
  Token_Keyword_If,
  Token_Keyword_Else,
  Token_Keyword_While,
  Token_Keyword_For,
  Token_Keyword_Break,
  Token_Keyword_Continue,
  Token_Keyword_Struct,
  Token_Keyword_Union,
  Token_Keyword_Enum,
  Token_Keyword_Typedef,
  Token_Keyword_Static,
  Token_Keyword_Void,
  Token_Keyword_Int,
  Token_Keyword_Char,
  Token_Keyword_Float,
  Token_Keyword_Double,
  Token_Keyword_Unsigned,
  Token_Keyword_Signed,
  Token_Keyword_Const,
  Token_Keyword_Extern,
  Token_Keyword_Switch,
  Token_Keyword_Case,
  Token_Keyword_Default,
  Token_Keyword_Sizeof,
  Token_Keyword_Inline,
  Token_Keyword_Do,
  Token_Keyword_Goto,
  Token_Keyword_Restrict,
  Token_Keyword_Volatile,
  Token_Keyword_Register,

  // Preprocessor
  Token_Preprocessor_Hash, // #

  // Operators
  Token_Plus,            // +
  Token_Plus_Assign,     // +=
  Token_Increment,       // ++
  Token_Minus,           // -
  Token_Minus_Assign,    // -=
  Token_Decrement,       // --
  Token_Arrow,           // ->
  Token_Multiply,        // *
  Token_Multiply_Assign, // *=
  Token_Divide,          // /
  Token_Divide_Assign,   // /=
  Token_Modulo,          // %
  Token_Modulo_Assign,   // %=

  Token_Assign,             // =
  Token_Equal,              // ==
  Token_Not,                // !
  Token_Not_Equal,          // !=
  Token_Less,               // <
  Token_Less_Equal,         // <=
  Token_Left_Shift,         // <<
  Token_Left_Shift_Assign,  // <<=
  Token_Greater,            // >
  Token_Greater_Equal,      // >=
  Token_Right_Shift,        // >>
  Token_Right_Shift_Assign, // >>=

  Token_Bit_And,         // &
  Token_Bit_And_Assign,  // &=
  Token_Logical_And,     // &&
  Token_Bit_Or,          // |
  Token_Bit_Or_Assign,   // |=
  Token_Logical_Or,      // ||

  Token_Bit_Xor,         // ^
  Token_Bit_Xor_Assign,  // ^=
  Token_Bit_Not,         // ~

  // Delimiters
  Token_Semicolon,        // ;
  Token_Comma,            // ,  
  Token_Dot,              // .
  Token_Colon,            // :
  Token_Question,         // ?

  Token_Open_Parenthesis,  // (
  Token_Close_Parenthesis, // )
  Token_Open_Brace,        // {
  Token_Close_Brace,       // }
  Token_Open_Bracket,      // [
  Token_Close_Bracket,     // ]

  // Special
  Token_Space,                // ' '
  Token_Tab,                  // /t
  Token_New_Line,              // \n, \r, or \r\n
  Token_Comment_Line,          // //
  Token_Comment_Block,         // /* ... */

  Token_End_Of_File,

  Token_Count, // Keep at the end of the enum
} Token_Type;

typedef struct Token {
  Token_Type type;
  String8 value;
  u32 start_offset;
  u32 end_offset;
  u32 line;
  u32 column;
} Token;

///////////////
// Lexer
typedef struct Lexer {
  Arena* arena;

  File_Data file;
  char8* file_start;
  char8* file_end;
  char8* current_character;

  u32 line;
  u32 column;

  Token current_token;
} Lexer;

typedef struct Token_Array {
  Token* tokens;
  u64 count ;
} Token_Array;
#define TOKEN_ARRAY_SIZE 4096

Token_Array tokenize_file(Lexer* lexer, String8 file_path); /* Initializes the lexer with workspace path */
Token       next_token(Lexer* lexer);

// Tokening
Token token_from_whitespace(Lexer* lexer);
Token token_from_comment(Lexer* lexer);
b32   token_from_operator(Lexer* lexer);
b32   token_from_delimiter(Lexer* lexer);
b32   token_from_braces(Lexer* lexer);
Token token_from_identifier_or_keyword(Lexer* lexer);
Token token_from_number(Lexer* lexer);
Token token_from_string(Lexer* lexer);
Token token_from_character(Lexer* lexer);

Token make_token_range(Lexer* lexer, Token_Type type, char8* start, char8* end);
Token make_token(Lexer* lexer, Token_Type type, u32 length);
Token_Type is_token_keyword(Token identifier_token); /* Checks if a token is an identifier token is a keyword */

// Manouvering
char8 peek_character(Lexer* lexer, u32 offset);            /* Returns next character without advancing */
Token peek_token(Lexer* lexer);                            /* Returns next token without advancing */
Token peek_token_skip_trivia(Lexer* lexer);                /* Returns next token without advancing */
void  advance(Lexer* lexer);                               /* Advances characters by 1 */
void  advance_by(Lexer* lexer, u32 count);                 /* Advances characters by count */
b32   advance_if_match(Lexer* lexer, char8 expected);      /* Advance if current character matches expected char */
b32   lexer_at_eof(Lexer* lexer);                          /* Checks if lexer is at the end of file */
u32   offset_of_character(Lexer* lexer, char8* character); /* Returns the offset into the file, of the given character */

b32 is_token_datatype(Token token);
b32 is_token_whitespace(Token token);
b32 is_token_trivia(Token_Type type); // TODO(Fz): Receive a token, not token_type

// Help
void token_print(Token token);

#endif // LEXER_H