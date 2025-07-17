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
void next_token(Lexer* lexer) {
  if (lexer->file.data.size == 0)  return;
  MemoryZeroStruct(&lexer->current_token);

  char8 c = current_character(lexer);
  if (lexer_at_eof(lexer)) {
    make_token(lexer, Token_End_Of_File, 1);
    return;
  } else if (char8_is_space(c)) {
    token_from_whitespace(lexer);
  } else if (c == '#') {
    make_token(lexer, Token_Preprocessor_Hash, 1);  
    advance_character(lexer);
  } else if ((c == '/' && (peek_character(lexer, 1) == '/' || peek_character(lexer, 1) == '*')) || (c == '*' && peek_character(lexer, 1) == '/')) {
    token_from_comment(lexer);
  } else if (char8_is_alpha(c) || c == '_') {
    token_from_identifier_or_keyword(lexer);
  } else if (isdigit(c)) {
    token_from_number(lexer);
  } else if (c == '"') {
    token_from_string(lexer);
  } else if (c == '\'') {
    token_from_character(lexer);
  } else if (token_from_operator(lexer)) {
  } else if (token_from_delimiter(lexer)) {
  } else if (token_from_braces(lexer)) {
  } else {
    //lexer->current_token.type = Token_Unknown;
    make_token(lexer, Token_Unknown, 1);
    advance_character(lexer);
  }

#if PRINT_TOKENS
  token_print(lexer);
#endif
}

b32 token_from_whitespace(Lexer* lexer) {
  char8 c = current_character(lexer);
  
  if (c == ' ') {
    make_token(lexer, Token_Space, 1);
    advance_character(lexer);
    return true;
  }
  
  if (c == '\t') {
    make_token(lexer, Token_Tab, 1);
    advance_character(lexer);
    return true;
  }
  
  if (c == '\n' || c == '\r') {
    char8* start = lexer->current_character;
    advance_character(lexer);

    if (c == '\r') {
      advance_if_match(lexer, '\n');
    }

    lexer->line += 1;
    lexer->column = 1;
    make_token_range(lexer, Token_New_Line, start, lexer->current_character);
    return true;
  }
  
  return false;
}

b32 token_from_comment(Lexer* lexer) {
  char8 c = current_character(lexer);
  
  if (c == '/' && peek_character(lexer, 1) == '/') {
    make_token(lexer, Token_Comment_Line, 2);
    advance_by(lexer, 2);
    return true;
  }
  
  if (c == '/' && peek_character(lexer, 1) == '*') {
    char8* start = lexer->current_character;
    make_token(lexer, Token_Comment_Block_Start, 2);
    advance_by(lexer, 2);
    return true;
  }
  
  if (c == '*' && peek_character(lexer, 1) == '/') {
    char8* start = lexer->current_character;
    make_token(lexer, Token_Comment_Block_End, 2);
    advance_by(lexer, 2);
    return true;
  }
  
  return false;
}

b32 token_from_operator(Lexer* lexer) {
  char8 c = current_character(lexer);
  char8 next = peek_character(lexer, 1);
  
  switch (c) {
    case '+':
      if (next == '+') {
        make_token(lexer, Token_Increment, 2);
        advance_by(lexer, 2);
      } else if (next == '=') {
        make_token(lexer, Token_Plus_Assign, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Plus, 1);
        advance_character(lexer);
      }
      return true;
      
    case '-':
      if (next == '-') {
        make_token(lexer, Token_Decrement, 2);
        advance_by(lexer, 2);
      } else if (next == '=') {
        make_token(lexer, Token_Minus_Assign, 2);
        advance_by(lexer, 2);
      } else if (next == '>') {
        make_token(lexer, Token_Arrow, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Minus, 1);
        advance_character(lexer);
      }
      return true;
      
    case '*':
      if (next == '=') {
        make_token(lexer, Token_Multiply_Assign, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Multiply, 1);
        advance_character(lexer);
      }
      return true;
      
    case '/':
      if (next == '=') {
        make_token(lexer, Token_Divide_Assign, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Divide, 1);
        advance_character(lexer);
      }
      return true;
      
    case '%':
      if (next == '=') {
        make_token(lexer, Token_Modulo_Assign, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Modulo, 1);
        advance_character(lexer);
      }
      return true;
      
    case '=':
      if (next == '=') {
        make_token(lexer, Token_Equal, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Assign, 1);
        advance_character(lexer);
      }
      return true;
      
    case '!':
      if (next == '=') {
        make_token(lexer, Token_Not_Equal, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Not, 1);
        advance_character(lexer);
      }
      return true;
      
    case '<':
      if (next == '=') {
        make_token(lexer, Token_Less_Equal, 2);
        advance_by(lexer, 2);
      } else if (next == '<') {
        if (peek_character(lexer, 2) == '=') {
          make_token(lexer, Token_Left_Shift_Assign, 3);
          advance_by(lexer, 3);
        } else {
          make_token(lexer, Token_Left_Shift, 2);
          advance_by(lexer, 2);
        }
      } else {
        make_token(lexer, Token_Less, 1);
        advance_character(lexer);
      }
      return true;
      
    case '>':
      if (next == '=') {
        make_token(lexer, Token_Greater_Equal, 2);
        advance_by(lexer, 2);
      } else if (next == '>') {
        if (peek_character(lexer, 2) == '=') {
          make_token(lexer, Token_Right_Shift_Assign, 3);
          advance_by(lexer, 3);
        } else {
          make_token(lexer, Token_Right_Shift, 2);
          advance_by(lexer, 2);
        }
      } else {
        make_token(lexer, Token_Greater, 1);
        advance_character(lexer);
      }
      return true;
      
    case '&':
      if (next == '&') {
        make_token(lexer, Token_Logical_And, 2);
        advance_by(lexer, 2);
      } else if (next == '=') {
        make_token(lexer, Token_Bit_And_Assign, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Bit_And, 1);
        advance_character(lexer);
      }
      return true;
      
    case '|':
      if (next == '|') {
        make_token(lexer, Token_Logical_Or, 2);
        advance_by(lexer, 2);
      } else if (next == '=') {
        make_token(lexer, Token_Bit_Or_Assign, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Bit_Or, 1);
        advance_character(lexer);
      }
      return true;
      
    case '^':
      if (next == '=') {
        make_token(lexer, Token_Bit_Xor_Assign, 2);
        advance_by(lexer, 2);
      } else {
        make_token(lexer, Token_Bit_Xor, 1);
        advance_character(lexer);
      }
      return true;
      
    case '~':
      make_token(lexer, Token_Bit_Not, 1);
      advance_character(lexer);
      return true;
  }
  
  return false;
}

b32 token_from_delimiter(Lexer* lexer) {
  char8 c = current_character(lexer);
  
  switch (c) {
    case ';':
      make_token(lexer, Token_Semicolon, 1);
      advance_character(lexer);
      return true;
    case ',':
      make_token(lexer, Token_Comma, 1);
      advance_character(lexer);
      return true;
    case '.':
      make_token(lexer, Token_Dot, 1);
      advance_character(lexer);
      return true;
    case ':':
      make_token(lexer, Token_Colon, 1);
      advance_character(lexer);
      return true;
    case '?':
      make_token(lexer, Token_Question, 1);
      advance_character(lexer);
      return true;
  }
  
  return false;
}

b32 token_from_braces(Lexer* lexer) {
  char8 c = current_character(lexer);
  
  switch (c) {
    case '(':
      make_token(lexer, Token_Open_Parenthesis, 1);
      advance_character(lexer);
      return true;
    case ')':
      make_token(lexer, Token_Close_Parenthesis, 1);
      advance_character(lexer);
      return true;
    case '{':
      make_token(lexer, Token_Open_Brace, 1);
      advance_character(lexer);
      return true;
    case '}':
      make_token(lexer, Token_Close_Brace, 1);
      advance_character(lexer);
      return true;
    case '[':
      make_token(lexer, Token_Open_Bracket, 1);
      advance_character(lexer);
      return true;
    case ']':
      make_token(lexer, Token_Close_Bracket, 1);
      advance_character(lexer);
      return true;
  }
  
  return false;
}

b32 token_from_identifier_or_keyword(Lexer* lexer) {
  char8 c = current_character(lexer);
  
  if (!char8_is_alpha(c) && c != '_') return false;
  
  char8* start = lexer->current_character;
  
  while (char8_is_alphanum(current_character(lexer)) || current_character(lexer) == '_') {
    advance_character(lexer);
  }
  
  make_token_range(lexer, Token_Identifier, start, lexer->current_character);
  
  Token_Type keyword_type = is_token_keyword(lexer->current_token);
  if (keyword_type != Token_Identifier) {
    lexer->current_token.type = keyword_type;
  }
  
  return true;
}

b32 token_from_number(Lexer* lexer) {
  char8 c = current_character(lexer);
  
  if (!isdigit(c)) {
    return false;
  }
  
  char8* start = lexer->current_character;
  Token_Type token_type = Token_Int_Literal;
  
  // Handle hex numbers
  if (c == '0' && (peek_character(lexer, 1) == 'x' || peek_character(lexer, 1) == 'X')) {
    advance_by(lexer, 2);
    while (isxdigit(current_character(lexer))) {
      advance_character(lexer);
    }
    token_type = Token_Hex_Literal;
  } else {
    // Handle decimal numbers
    while (isdigit(current_character(lexer))) {
      advance_character(lexer);
    }
    
    // Handle floating point
    if (current_character(lexer) == '.') {
      advance_character(lexer);
      while (isdigit(current_character(lexer))) {
        advance_character(lexer);
      }
      token_type = Token_Float_Literal;
    }
    
    // Handle scientific notation
    if (current_character(lexer) == 'e' || current_character(lexer) == 'E') {
      advance_character(lexer);
      if (current_character(lexer) == '+' || current_character(lexer) == '-') {
        advance_character(lexer);
      }
      while (isdigit(current_character(lexer))) {
        advance_character(lexer);
      }
      token_type = Token_Float_Literal;
    }
  }
  
  // Handle suffixes
  char8 suffix = current_character(lexer);
  if (suffix == 'f' || suffix == 'F') {
    advance_character(lexer);
    token_type = Token_Float_Literal;
  } else if (suffix == 'l' || suffix == 'L' || suffix == 'u' || suffix == 'U') {
    advance_character(lexer);
    // Handle UL, LU combinations
    char8 next_suffix = current_character(lexer);
    if ((suffix == 'u' || suffix == 'U') && (next_suffix == 'l' || next_suffix == 'L')) {
      advance_character(lexer);
    } else if ((suffix == 'l' || suffix == 'L') && (next_suffix == 'u' || next_suffix == 'U')) {
      advance_character(lexer);
    }
  }
  
  make_token_range(lexer, token_type, start, lexer->current_character);
  return true;
}


b32 token_from_string(Lexer* lexer) {
  if (current_character(lexer) != '"') {
    return false;
  }
  
  char8* start = lexer->current_character;
  advance_character(lexer); // Skip opening quote
  
  while (!lexer_at_eof(lexer) && current_character(lexer) != '"') {
    if (current_character(lexer) == '\\') {
      advance_character(lexer); // Skip escape character
      if (!lexer_at_eof(lexer)) {
        advance_character(lexer); // Skip escaped character
      }
    } else {
      advance_character(lexer);
    }
  }
  
  if (current_character(lexer) == '"') {
    advance_character(lexer); // Skip closing quote
  }
  
  make_token_range(lexer, Token_String_Literal, start, lexer->current_character);
  return true;
}

b32 token_from_character(Lexer* lexer) {
  if (current_character(lexer) != '\'') {
    return false;
  }
  
  char8* start = lexer->current_character;
  advance_character(lexer); // Skip opening quote
  
  if (!lexer_at_eof(lexer)) {
    if (current_character(lexer) == '\\') {
      advance_character(lexer); // Skip escape character
      if (!lexer_at_eof(lexer)) {
        advance_character(lexer); // Skip escaped character
      }
    } else {
      advance_character(lexer); // Skip character
    }
  }
  
  if (current_character(lexer) == '\'') {
    advance_character(lexer); // Skip closing quote
  }
  
  make_token_range(lexer, Token_Char_Literal, start, lexer->current_character);
  return true;
}

void make_token_range(Lexer* lexer, Token_Type type, char8* start, char8* end) {
  lexer->current_token.type         = type;
  lexer->current_token.value.str    = start;
  lexer->current_token.value.size   = (u32)(end - start);
  lexer->current_token.start_offset = offset_of_character(lexer, start);
  lexer->current_token.end_offset   = offset_of_character(lexer, end);
}

void make_token(Lexer* lexer, Token_Type type, u32 length) {
  lexer->current_token.type         = type;
  lexer->current_token.value.str    = lexer->current_character;
  lexer->current_token.value.size   = length;
  lexer->current_token.start_offset = offset_of_character(lexer, lexer->current_character);
  lexer->current_token.end_offset   = offset_of_character(lexer, lexer->current_character + length);
}

Token_Type is_token_keyword(Token identifier_token) {
  String8 value = identifier_token.value;
  
  if (string8_equal(value, Str8("return")))    return Token_Keyword_Return;
  if (string8_equal(value, Str8("if")))        return Token_Keyword_If;
  if (string8_equal(value, Str8("else")))      return Token_Keyword_Else;
  if (string8_equal(value, Str8("while")))     return Token_Keyword_While;
  if (string8_equal(value, Str8("for")))       return Token_Keyword_For;
  if (string8_equal(value, Str8("break")))     return Token_Keyword_Break;
  if (string8_equal(value, Str8("continue")))  return Token_Keyword_Continue;
  if (string8_equal(value, Str8("struct")))    return Token_Keyword_Struct;
  if (string8_equal(value, Str8("union")))     return Token_Keyword_Union;
  if (string8_equal(value, Str8("enum")))      return Token_Keyword_Enum;
  if (string8_equal(value, Str8("typedef")))   return Token_Keyword_Typedef;
  if (string8_equal(value, Str8("static")))    return Token_Keyword_Static;
  if (string8_equal(value, Str8("void")))      return Token_Keyword_Void;
  if (string8_equal(value, Str8("int")))       return Token_Keyword_Int;
  if (string8_equal(value, Str8("char")))      return Token_Keyword_Char;
  if (string8_equal(value, Str8("float")))     return Token_Keyword_Float;
  if (string8_equal(value, Str8("double")))    return Token_Keyword_Double;
  if (string8_equal(value, Str8("unsigned")))  return Token_Keyword_Unsigned;
  if (string8_equal(value, Str8("signed")))    return Token_Keyword_Signed;
  if (string8_equal(value, Str8("const")))     return Token_Keyword_Const;
  if (string8_equal(value, Str8("extern")))    return Token_Keyword_Extern;
  if (string8_equal(value, Str8("switch")))    return Token_Keyword_Switch;
  if (string8_equal(value, Str8("case")))      return Token_Keyword_Case;
  if (string8_equal(value, Str8("default")))   return Token_Keyword_Default;
  if (string8_equal(value, Str8("sizeof")))    return Token_Keyword_Sizeof;
  if (string8_equal(value, Str8("inline")))    return Token_Keyword_Inline;
  if (string8_equal(value, Str8("do")))        return Token_Keyword_Do;
  if (string8_equal(value, Str8("goto")))      return Token_Keyword_Goto;
  if (string8_equal(value, Str8("restrict")))  return Token_Keyword_Restrict;
  if (string8_equal(value, Str8("volatile")))  return Token_Keyword_Volatile;
  if (string8_equal(value, Str8("register")))  return Token_Keyword_Register;
    
  return Token_Identifier;
}

b32 lexer_at_eof(Lexer* lexer) {
  return lexer->current_character >= lexer->file_end;
}

u32 offset_of_character(Lexer* lexer, char8* character) {
  return (u32)(character - lexer->file_start);
}

char8 peek_character(Lexer* lexer, u32 offset) {
  if (lexer->current_character + offset >= lexer->file_end) {
    return '\0';
  }
  return lexer->current_character[offset];
}

Token lexer_peek_token_skip_whitespace(Lexer* lexer) {
  // Save current state (No MemoryCopy because we have all files loaded).
  char8* saved_pos    = lexer->current_character;
  u32 saved_line      = lexer->line;
  u32 saved_column    = lexer->column;
  Token saved_current = lexer->current_token;
    
  // Get next token
  next_token(lexer);
  Token next_non_whitespace_token = lexer->current_token;
  while (is_token_whitespace(next_non_whitespace_token)) {
    next_token(lexer);
    next_non_whitespace_token = lexer->current_token;
  }
  
  // Restore state
  lexer->current_character = saved_pos;
  lexer->line              = saved_line;
  lexer->column            = saved_column;
  lexer->current_token     = saved_current;
    
  return next_non_whitespace_token;
}

Token peek_token(Lexer* lexer) {
  // Save current state (No MemoryCopy because we have all files loaded).
  char8* saved_pos    = lexer->current_character;
  u32 saved_line      = lexer->line;
  u32 saved_column    = lexer->column;
  Token saved_current = lexer->current_token;
    
  // Get next token
  next_token(lexer);
  Token result = lexer->current_token;
    
  // Restore state
  lexer->current_character = saved_pos;
  lexer->line              = saved_line;
  lexer->column            = saved_column;
  lexer->current_token     = saved_current;
    
  return result;
}

Token peek_token_skip_trivia(Lexer* lexer) {
  char8* save_cursor  = lexer->current_character;
  u32 save_line       = lexer->line;
  u32 save_column     = lexer->column;
  Token save_token    = lexer->current_token;

  Token result;
  do {
    next_token(lexer);
    result = lexer->current_token;
  } while (is_token_trivia(result.type));

  // Restore lexer state
  lexer->current_character = save_cursor;
  lexer->line              = save_line;
  lexer->column            = save_column;
  lexer->current_token     = save_token;

  return result;
}

char8 current_character(Lexer* lexer) {
  return peek_character(lexer, 0);
}

void advance_character(Lexer* lexer) {
  if (lexer->current_character >= lexer->file_end)  return;
    
  if (*(lexer->current_character) == '\n') {
    lexer->line += 1;
    lexer->column = 1;
  } else {
    lexer->column += 1;
  }
    
  lexer->current_character += 1;
}

void advance_by(Lexer* lexer, u32 count) {
  for (u32 i = 0; i < count && lexer->current_character < lexer->file_end; i += 1) {
    advance_character(lexer);
  }
}

b32 advance_if_match(Lexer* lexer, char8 expected) {
  if (current_character(lexer) == expected) {
    advance_character(lexer);
    return true;
  }
  return false;
}

b32 is_token_datatype(Token token) {
  b32 result  = (token.type == Token_Keyword_Int      ||
                 token.type == Token_Keyword_Float    ||
                 token.type == Token_Keyword_Double   ||
                 token.type == Token_Keyword_Char     ||
                 token.type == Token_Keyword_Signed   ||
                 token.type == Token_Keyword_Unsigned ||
                 token.type == Token_Keyword_Void);
  return result;
}

b32 is_token_whitespace(Token token) {
  b32 result = (token.type == Token_Space ||
                token.type == Token_Tab   ||
                token.type == Token_New_Line);
  return result;
}

b32 is_token_trivia(Token_Type type) {
  return (type == Token_Space ||
          type == Token_Tab ||
          type == Token_New_Line ||
          type == Token_Comment_Line ||
          type == Token_Comment_Block_Start ||
          type == Token_Comment_Block_End);
}

void token_print(Lexer* lexer) {
  if (!lexer) return;

  // Safety: check enum range
  if (lexer->current_token.type >= Token_Count) {
    printf("xx Token_Type out of range: %d\n", lexer->current_token.type);
    return;
  }

  // Print token type
  printf("%s: ", token_type_names[lexer->current_token.type]);
  for (u32 i = strlen(token_type_names[lexer->current_token.type]); i < 26; i += 1) {
    printf(" ");
  }

  Assert(lexer->current_token.start_offset + lexer->current_token.value.size == lexer->current_token.end_offset);

  // Print token value if it has one (non-empty string)
  if (lexer->current_token.value.size > 0 && lexer->current_token.value.str) {
    if (lexer->current_token.type == Token_New_Line) {
      printf("Token value: '\\n'");
    } else {
      printf("Token value: '%.*s'", (s32)lexer->current_token.value.size, lexer->current_token.value.str);
    }
    for (u32 i = lexer->current_token.value.size; i < 16; i += 1) {
      printf(" ");
    }
    printf("StartEnd: [%d, %d]", lexer->current_token.start_offset, lexer->current_token.end_offset);
  }
  printf("\n");

  return;
}
