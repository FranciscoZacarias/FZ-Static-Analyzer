#ifndef LEXER_H
#define LEXER_H

///////////////
// Token

static const char* token_type_names[] = {
  "Token_Unknown",

  // Literals
  "Token_Identifier",
  "Token_Number",
  "Token_String",
  "Token_Char",

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
  "Token_Keyword_Auto",
  "Token_Keyword_Register",

  // Preprocessor
  "Token_Preprocessor_Hash",    // #
  "Token_Preprocessor_Include", // #include
  "Token_Preprocessor_Define",  // #define
  "Token_Preprocessor_Ifdef",   // #ifdef
  "Token_Preprocessor_Ifndef",  // #ifndef
  "Token_Preprocessor_Endif",   // #endif
  "Token_Preprocessor_Else",    // #else
  "Token_Preprocessor_Elif",    // #elif
  "Token_Preprocessor_Error",   // #error
  "Token_Preprocessor_Pragma",  // #pragma

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

  "Token_Left_Parenthesis",  // (
  "Token_Right_Parenthesis", // )
  "Token_Left_Brace",        // {
  "Token_Right_Brace",       // }
  "Token_Left_Bracket",      // [
  "Token_Right_Bracket",     // ]

  // Special
  "Token_Spaces",                // ' '
  "Token_Tabs",                  // /t
  "Token_New_Line",              // \n, \r, or \r\n
  "Token_Comment_Line",          // //
  "Token_Comment_Block_Start",   // /*
  "Token_Comment_Block_End",     // */

  "Token_End_Of_File",

  "Token_Count", // Keep at the end of the enum
};

typedef enum Token_Type {
  Token_Unknown = 0,

  // Literals
  Token_Identifier,
  Token_Number,
  Token_String,
  Token_Char,

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
  Token_Keyword_Auto,
  Token_Keyword_Register,

  // Preprocessor
  Token_Preprocessor_Hash,    // #
  Token_Preprocessor_Include, // #include
  Token_Preprocessor_Define,  // #define
  Token_Preprocessor_Ifdef,   // #ifdef
  Token_Preprocessor_Ifndef,  // #ifndef
  Token_Preprocessor_Endif,   // #endif
  Token_Preprocessor_Else,    // #else
  Token_Preprocessor_Elif,    // #elif
  Token_Preprocessor_Error,   // #error
  Token_Preprocessor_Pragma,  // #pragma

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

  Token_Left_Parenthesis,  // (
  Token_Right_Parenthesis, // )
  Token_Left_Brace,        // {
  Token_Right_Brace,       // }
  Token_Left_Bracket,      // [
  Token_Right_Bracket,     // ]

  // Special
  Token_Spaces,                // ' '
  Token_Tabs,                  // /t
  Token_New_Line,              // \n, \r, or \r\n
  Token_Comment_Line,          // //
  Token_Comment_Block_Start,   // /*
  Token_Comment_Block_End,     // */

  Token_End_Of_File,

  Token_Count, // Keep at the end of the enum
} Token_Type;

typedef struct Token {
  Token_Type type;
  String8 value;
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

void lexer_init(Lexer* lexer, String8 file_path); /* Initializes the lexer with workspace path */
void lexer_get_next_token(Lexer* lexer);

// Tokening
b32 lexer_get_whitespace(Lexer* lexer);
b32 lexer_get_comment(Lexer* lexer);
b32 lexer_get_operator(Lexer* lexer);
b32 lexer_get_delimiter(Lexer* lexer);
b32 lexer_get_braces(Lexer* lexer);
b32 lexer_get_identifier_or_keyword(Lexer* lexer);
b32 lexer_get_number(Lexer* lexer);
b32 lexer_get_string(Lexer* lexer);
b32 lexer_get_character(Lexer* lexer);

Token lexer_make_token_range(Lexer* lexer, Token_Type type, char8* start, char8* end);
Token lexer_make_token_current(Lexer* lexer, Token_Type type, u32 length);
Token lexer_is_token_keyword(Token identifier_token); /* Checks if a token is an identifier token is a keyword */

// Manouvering
char8 lexer_peek(Lexer* lexer, u32 offset);                  /* Returns next character without advancing */
Token lexer_peek_token(Lexer* lexer, u32 offset);            /* Returns next token without advancing */
char8 lexer_current(Lexer* lexer);                           /* Returns current character */
void  lexer_advance(Lexer* lexer);                           /* Advances characters by 1 */
void  lexer_advance_by(Lexer* lexer, u32 count);             /* Advances characters by count */
b32   lexer_advance_if_match(Lexer* lexer, char8 expected);  /* Advance if current character matches expected char */
b32   lexer_is_at_eof(Lexer* lexer);                         /* Checks if lexer is at the end of file */

// Help
b32 lexer_print_current_token(Lexer* lexer);

#endif // LEXER_H