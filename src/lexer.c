///////////////
// Lexer
void lexer_init(Lexer* lexer, String8 file_path) {
  MemoryZeroStruct(lexer);
 
  lexer->arena             = arena_init();
  lexer->file              = file_load(lexer->arena, file_path);
  lexer->current_character = lexer->file.data.str;
  lexer->file_start        = lexer->file.data.str;
  lexer->file_end          = lexer->file.data.str + lexer->file.data.size;
  lexer->line              = 1;
  lexer->column            = 1;

  lexer->current_token.type  = Token_Unknown;
  lexer->current_token.value = Str8("");
}

// Parse token
// - Whitespace and Comments (skip/consume)
// - Multi-character operators (longest match first)
// - Keywords vs Identifiers
// - Number literals
// - String and character literals
// - Single-character operators and punctuation
// - Unknown/Error tokens
void lexer_get_next_token(Lexer* lexer) {
  if (lexer->file.data.size == 0)  return;
  MemoryZeroStruct(&lexer->current_token);

  if (lexer_is_at_eof(lexer)) {
    lexer->current_token = (Token){Token_End_Of_File, {0}};
    return;
  }
  
  char8 c = lexer_current(lexer);
  if (lexer_is_at_eof(lexer)) {
    lexer->current_token.type = Token_End_Of_File;
  } else if (char8_is_space(c)) {
    lexer_get_whitespace(lexer);
  } else if (c == '#') {
    lexer->current_token = lexer_make_token_current(lexer, Token_Preprocessor_Hash, 1);
    lexer_advance(lexer);
  } else if ((c == '/' && (lexer_peek(lexer, 1) == '/' || lexer_peek(lexer, 1) == '*')) || (c == '*' && lexer_peek(lexer, 1) == '/')) {
    lexer_get_comment(lexer);
  } else if (char8_is_alpha(c) || c == '_') {
    lexer_get_identifier_or_keyword(lexer);
  } else if (isdigit(c)) {
    lexer_get_number(lexer);
  } else if (c == '"') {
    lexer_get_string(lexer);
  } else if (c == '\'') {
    lexer_get_character(lexer);
  } else if (lexer_get_operator(lexer)) {
  } else if (lexer_get_delimiter(lexer)) {
  } else if (lexer_get_braces(lexer)) {
  } else {
    lexer->current_token.type = Token_Unknown;
    lexer_advance(lexer);
  }

#if PRINT_TOKENS
  lexer_print_current_token(lexer);
#endif

  return;
}

b32 lexer_get_whitespace(Lexer* lexer) {
  char8 c = lexer_current(lexer);
  
  if (c == ' ') {
    lexer->current_token = lexer_make_token_current(lexer, Token_Space, 1);
    lexer_advance(lexer);
    return true;
  }
  
  if (c == '\t') {
    lexer->current_token = lexer_make_token_current(lexer, Token_Tab, 1);
    lexer_advance(lexer);
    return true;
  }
  
  if (c == '\n' || c == '\r') {
    char8* start = lexer->current_character;
    lexer_advance(lexer);

    if (c == '\r') {
      lexer_advance_if_match(lexer, '\n');
    }

    lexer->line += 1;
    lexer->column = 1;
    lexer->current_token = lexer_make_token_range(lexer, Token_New_Line, start, lexer->current_character);
    return true;
  }
  
  return false;
}

b32 lexer_get_comment(Lexer* lexer) {
  char8 c = lexer_current(lexer);
  
  if (c == '/' && lexer_peek(lexer, 1) == '/') {
    lexer->current_token = lexer_make_token_current(lexer, Token_Comment_Line, 2);
    lexer_advance_by(lexer, 2);
    return true;
  }
  
  if (c == '/' && lexer_peek(lexer, 1) == '*') {
    char8* start = lexer->current_character;
    lexer->current_token = lexer_make_token_current(lexer, Token_Comment_Block_Start, 2);
    lexer_advance_by(lexer, 2);
    return true;
  }
  
  if (c == '*' && lexer_peek(lexer, 1) == '/') {
    char8* start = lexer->current_character;
    lexer->current_token = lexer_make_token_current(lexer, Token_Comment_Block_End, 2);
    lexer_advance_by(lexer, 2);
    return true;
  }
  
  return false;
}

b32 lexer_get_operator(Lexer* lexer) {
  char8 c = lexer_current(lexer);
  char8 next = lexer_peek(lexer, 1);
  
  switch (c) {
    case '+':
      if (next == '+') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Increment, 2);
        lexer_advance_by(lexer, 2);
      } else if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Plus_Assign, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Plus, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '-':
      if (next == '-') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Decrement, 2);
        lexer_advance_by(lexer, 2);
      } else if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Minus_Assign, 2);
        lexer_advance_by(lexer, 2);
      } else if (next == '>') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Arrow, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Minus, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '*':
      if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Multiply_Assign, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Multiply, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '/':
      if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Divide_Assign, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Divide, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '%':
      if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Modulo_Assign, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Modulo, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '=':
      if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Equal, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Assign, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '!':
      if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Not_Equal, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Not, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '<':
      if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Less_Equal, 2);
        lexer_advance_by(lexer, 2);
      } else if (next == '<') {
        if (lexer_peek(lexer, 2) == '=') {
          lexer->current_token = lexer_make_token_current(lexer, Token_Left_Shift_Assign, 3);
          lexer_advance_by(lexer, 3);
        } else {
          lexer->current_token = lexer_make_token_current(lexer, Token_Left_Shift, 2);
          lexer_advance_by(lexer, 2);
        }
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Less, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '>':
      if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Greater_Equal, 2);
        lexer_advance_by(lexer, 2);
      } else if (next == '>') {
        if (lexer_peek(lexer, 2) == '=') {
          lexer->current_token = lexer_make_token_current(lexer, Token_Right_Shift_Assign, 3);
          lexer_advance_by(lexer, 3);
        } else {
          lexer->current_token = lexer_make_token_current(lexer, Token_Right_Shift, 2);
          lexer_advance_by(lexer, 2);
        }
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Greater, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '&':
      if (next == '&') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Logical_And, 2);
        lexer_advance_by(lexer, 2);
      } else if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Bit_And_Assign, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Bit_And, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '|':
      if (next == '|') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Logical_Or, 2);
        lexer_advance_by(lexer, 2);
      } else if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Bit_Or_Assign, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Bit_Or, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '^':
      if (next == '=') {
        lexer->current_token = lexer_make_token_current(lexer, Token_Bit_Xor_Assign, 2);
        lexer_advance_by(lexer, 2);
      } else {
        lexer->current_token = lexer_make_token_current(lexer, Token_Bit_Xor, 1);
        lexer_advance(lexer);
      }
      return true;
      
    case '~':
      lexer->current_token = lexer_make_token_current(lexer, Token_Bit_Not, 1);
      lexer_advance(lexer);
      return true;
  }
  
  return false;
}

b32 lexer_get_delimiter(Lexer* lexer) {
  char8 c = lexer_current(lexer);
  
  switch (c) {
    case ';':
      lexer->current_token = lexer_make_token_current(lexer, Token_Semicolon, 1);
      lexer_advance(lexer);
      return true;
    case ',':
      lexer->current_token = lexer_make_token_current(lexer, Token_Comma, 1);
      lexer_advance(lexer);
      return true;
    case '.':
      lexer->current_token = lexer_make_token_current(lexer, Token_Dot, 1);
      lexer_advance(lexer);
      return true;
    case ':':
      lexer->current_token = lexer_make_token_current(lexer, Token_Colon, 1);
      lexer_advance(lexer);
      return true;
    case '?':
      lexer->current_token = lexer_make_token_current(lexer, Token_Question, 1);
      lexer_advance(lexer);
      return true;
  }
  
  return false;
}

b32 lexer_get_braces(Lexer* lexer) {
  char8 c = lexer_current(lexer);
  
  switch (c) {
    case '(':
      lexer->current_token = lexer_make_token_current(lexer, Token_Open_Parenthesis, 1);
      lexer_advance(lexer);
      return true;
    case ')':
      lexer->current_token = lexer_make_token_current(lexer, Token_Close_Parenthesis, 1);
      lexer_advance(lexer);
      return true;
    case '{':
      lexer->current_token = lexer_make_token_current(lexer, Token_Open_Brace, 1);
      lexer_advance(lexer);
      return true;
    case '}':
      lexer->current_token = lexer_make_token_current(lexer, Token_Close_Brace, 1);
      lexer_advance(lexer);
      return true;
    case '[':
      lexer->current_token = lexer_make_token_current(lexer, Token_Open_Bracket, 1);
      lexer_advance(lexer);
      return true;
    case ']':
      lexer->current_token = lexer_make_token_current(lexer, Token_Close_Bracket, 1);
      lexer_advance(lexer);
      return true;
  }
  
  return false;
}

b32 lexer_get_identifier_or_keyword(Lexer* lexer) {
  char8 c = lexer_current(lexer);
  
  if (!char8_is_alpha(c) && c != '_')  return false;
  
  char8* start = lexer->current_character;
  
  while (char8_is_alphanum(lexer_current(lexer)) || lexer_current(lexer) == '_') {
    lexer_advance(lexer);
  }
  
  Token identifier = lexer_make_token_range(lexer, Token_Identifier, start, lexer->current_character);
  Token keyword_check = lexer_is_token_keyword(identifier);
  
  if (keyword_check.type != Token_Identifier) {
    lexer->current_token = keyword_check;
  } else {
    lexer->current_token = identifier;
  }
  
  return true;
}

b32 lexer_get_number(Lexer* lexer) {
  char8 c = lexer_current(lexer);
  
  if (!isdigit(c)) {
    return false;
  }
  
  char8* start = lexer->current_character;
  
  // Handle hex numbers
  if (c == '0' && (lexer_peek(lexer, 1) == 'x' || lexer_peek(lexer, 1) == 'X')) {
    lexer_advance_by(lexer, 2);
    while (isxdigit(lexer_current(lexer))) {
      lexer_advance(lexer);
    }
  } else {
    // Handle decimal numbers
    while (isdigit(lexer_current(lexer))) {
      lexer_advance(lexer);
    }
    
    // Handle floating point
    if (lexer_current(lexer) == '.') {
      lexer_advance(lexer);
      while (isdigit(lexer_current(lexer))) {
        lexer_advance(lexer);
      }
    }
    
    // Handle scientific notation
    if (lexer_current(lexer) == 'e' || lexer_current(lexer) == 'E') {
      lexer_advance(lexer);
      if (lexer_current(lexer) == '+' || lexer_current(lexer) == '-') {
        lexer_advance(lexer);
      }
      while (isdigit(lexer_current(lexer))) {
        lexer_advance(lexer);
      }
    }
  }
  
  // Handle suffixes (f, F, l, L, u, U, etc.)
  char8 suffix = lexer_current(lexer);
  if (suffix == 'f' || suffix == 'F' || suffix == 'l' || suffix == 'L' || 
      suffix == 'u' || suffix == 'U') {
    lexer_advance(lexer);
    // Handle UL, LU combinations
    char8 next_suffix = lexer_current(lexer);
    if ((suffix == 'u' || suffix == 'U') && (next_suffix == 'l' || next_suffix == 'L')) {
      lexer_advance(lexer);
    } else if ((suffix == 'l' || suffix == 'L') && (next_suffix == 'u' || next_suffix == 'U')) {
      lexer_advance(lexer);
    }
  }
  
  lexer->current_token = lexer_make_token_range(lexer, Token_Number, start, lexer->current_character);
  return true;
}

b32 lexer_get_string(Lexer* lexer) {
  if (lexer_current(lexer) != '"') {
    return false;
  }
  
  char8* start = lexer->current_character;
  lexer_advance(lexer); // Skip opening quote
  
  while (!lexer_is_at_eof(lexer) && lexer_current(lexer) != '"') {
    if (lexer_current(lexer) == '\\') {
      lexer_advance(lexer); // Skip escape character
      if (!lexer_is_at_eof(lexer)) {
        lexer_advance(lexer); // Skip escaped character
      }
    } else {
      lexer_advance(lexer);
    }
  }
  
  if (lexer_current(lexer) == '"') {
    lexer_advance(lexer); // Skip closing quote
  }
  
  lexer->current_token = lexer_make_token_range(lexer, Token_String_Literal, start, lexer->current_character);
  return true;
}

b32 lexer_get_character(Lexer* lexer) {
  if (lexer_current(lexer) != '\'') {
    return false;
  }
  
  char8* start = lexer->current_character;
  lexer_advance(lexer); // Skip opening quote
  
  if (!lexer_is_at_eof(lexer)) {
    if (lexer_current(lexer) == '\\') {
      lexer_advance(lexer); // Skip escape character
      if (!lexer_is_at_eof(lexer)) {
        lexer_advance(lexer); // Skip escaped character
      }
    } else {
      lexer_advance(lexer); // Skip character
    }
  }
  
  if (lexer_current(lexer) == '\'') {
    lexer_advance(lexer); // Skip closing quote
  }
  
  lexer->current_token = lexer_make_token_range(lexer, Token_Char, start, lexer->current_character);
  return true;
}

Token lexer_make_token_range(Lexer* lexer, Token_Type type, char8* start, char8* end) {
  Token token;
  token.type       = type;
  token.value.str  = start;
  token.value.size = (u32)(end - start);
  return token;
}

Token lexer_make_token_current(Lexer* lexer, Token_Type type, u32 length) {
  Token token;
  token.type       = type;
  token.value.str  = lexer->current_character;
  token.value.size = length;
  return token;
}

Token lexer_is_token_keyword(Token identifier_token) {
  String8 value = identifier_token.value;
  
  if (string8_equal(value, Str8("return")))    return (Token){Token_Keyword_Return,   value};
  if (string8_equal(value, Str8("if")))        return (Token){Token_Keyword_If,       value};
  if (string8_equal(value, Str8("else")))      return (Token){Token_Keyword_Else,     value};
  if (string8_equal(value, Str8("while")))     return (Token){Token_Keyword_While,    value};
  if (string8_equal(value, Str8("for")))       return (Token){Token_Keyword_For,      value};
  if (string8_equal(value, Str8("break")))     return (Token){Token_Keyword_Break,    value};
  if (string8_equal(value, Str8("continue")))  return (Token){Token_Keyword_Continue, value};
  if (string8_equal(value, Str8("struct")))    return (Token){Token_Keyword_Struct,   value};
  if (string8_equal(value, Str8("union")))     return (Token){Token_Keyword_Union,    value};
  if (string8_equal(value, Str8("enum")))      return (Token){Token_Keyword_Enum,     value};
  if (string8_equal(value, Str8("typedef")))   return (Token){Token_Keyword_Typedef,  value};
  if (string8_equal(value, Str8("static")))    return (Token){Token_Keyword_Static,   value};
  if (string8_equal(value, Str8("void")))      return (Token){Token_Keyword_Void,     value};
  if (string8_equal(value, Str8("int")))       return (Token){Token_Keyword_Int,      value};
  if (string8_equal(value, Str8("char")))      return (Token){Token_Keyword_Char,     value};
  if (string8_equal(value, Str8("float")))     return (Token){Token_Keyword_Float,    value};
  if (string8_equal(value, Str8("double")))    return (Token){Token_Keyword_Double,   value};
  if (string8_equal(value, Str8("unsigned")))  return (Token){Token_Keyword_Unsigned, value};
  if (string8_equal(value, Str8("signed")))    return (Token){Token_Keyword_Signed,   value};
  if (string8_equal(value, Str8("const")))     return (Token){Token_Keyword_Const,    value};
  if (string8_equal(value, Str8("extern")))    return (Token){Token_Keyword_Extern,   value};
  if (string8_equal(value, Str8("switch")))    return (Token){Token_Keyword_Switch,   value};
  if (string8_equal(value, Str8("case")))      return (Token){Token_Keyword_Case,     value};
  if (string8_equal(value, Str8("default")))   return (Token){Token_Keyword_Default,  value};
  if (string8_equal(value, Str8("sizeof")))    return (Token){Token_Keyword_Sizeof,   value};
  if (string8_equal(value, Str8("inline")))    return (Token){Token_Keyword_Inline,   value};
  if (string8_equal(value, Str8("do")))        return (Token){Token_Keyword_Do,       value};
  if (string8_equal(value, Str8("goto")))      return (Token){Token_Keyword_Goto,     value};
  if (string8_equal(value, Str8("restrict")))  return (Token){Token_Keyword_Restrict, value};
  if (string8_equal(value, Str8("volatile")))  return (Token){Token_Keyword_Volatile, value};
  if (string8_equal(value, Str8("register")))  return (Token){Token_Keyword_Register, value};
    
  return identifier_token;
}

b32 lexer_is_at_eof(Lexer* lexer) {
  return lexer->current_character >= lexer->file_end;
}

char8 lexer_peek(Lexer* lexer, u32 offset) {
  if (lexer->current_character + offset >= lexer->file_end) {
    return '\0';
  }
  return lexer->current_character[offset];
}

Token lexer_peek_token(Lexer* lexer, u32 offset) {
  // Save current state
  char8* saved_pos    = lexer->current_character;
  u32 saved_line      = lexer->line;
  u32 saved_column    = lexer->column;
  Token saved_current = lexer->current_token;
    
  // Get next token
  lexer_get_next_token(lexer);
  Token next_token = lexer->current_token;
    
  // Restore state
  lexer->current_character = saved_pos;
  lexer->line              = saved_line;
  lexer->column            = saved_column;
  lexer->current_token     = saved_current;
    
  return next_token;
}

char8 lexer_current(Lexer* lexer) {
  return lexer_peek(lexer, 0);
}

void lexer_advance(Lexer* lexer) {
  if (lexer->current_character >= lexer->file_end)  return;
    
  if (*lexer->current_character == '\n') {
    lexer->line += 1;
    lexer->column = 1;
  } else {
    lexer->column += 1;
  }
    
  lexer->current_character += 1;
}

void lexer_advance_by(Lexer* lexer, u32 count) {
  for (u32 i = 0; i < count && lexer->current_character < lexer->file_end; i += 1) {
    lexer_advance(lexer);
  }
}

b32 lexer_advance_if_match(Lexer* lexer, char8 expected) {
  if (lexer_current(lexer) == expected) {
    lexer_advance(lexer);
    return true;
  }
  return false;
}

b32 lexer_print_current_token(Lexer* lexer) {
  if (!lexer) return false;

  // Safety: check enum range
  if (lexer->current_token.type >= Token_Count) {
    printf("xx Token_Type out of range: %d\n", lexer->current_token.type);
    return false;
  }

  // Print token type
  printf("%s: ", token_type_names[lexer->current_token.type]);
  for (u32 i = strlen(token_type_names[lexer->current_token.type]); i < 26; i += 1) {
    printf(" ");
  }

  // Print token value if it has one (non-empty string)
  if (lexer->current_token.value.size > 0 && lexer->current_token.value.str) {
    if (lexer->current_token.type != Token_Space && lexer->current_token.type != Token_Tab && lexer->current_token.type != Token_New_Line) {
      printf("Token value: %.*s", (s32)lexer->current_token.value.size, lexer->current_token.value.str);
    }
  }
  printf("\n");

  return true;
}
