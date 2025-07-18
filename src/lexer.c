///////////////
// Lexer
Token_Array load_all_tokens(Lexer* lexer, String8 file_path) {
  MemoryZeroStruct(lexer);
 
  lexer->arena               = arena_init();
  lexer->file                = file_load(lexer->arena, file_path);
  lexer->current_character   = lexer->file.data.str;
  lexer->file_start          = lexer->file.data.str;
  lexer->file_end            = lexer->file.data.str + lexer->file.data.size;
  lexer->line                = 1;
  lexer->column              = 1;
  lexer->current_token.type  = Token_Unknown;
  lexer->current_token.value = Str8("");

  Token_Array result = {0};
  Token* list = ArenaPush(lexer->arena, Token, TOKEN_ARRAY_SIZE);
  u64 count   = 0;

  for (;;) {
    Token token = next_token(lexer);
#if PRINT_TOKENS
    token_print(token);
#endif
    list[count] = token;
    count += 1;
    if (token.type == Token_End_Of_File) {
      break;
    }
  }

  result.tokens = list;
  result.count  = count;
  return result;
}

// Parse token
// - Whitespace and Comments (skip/consume)
// - Multi-character operators (longest match first)
// - Keywords vs Identifiers
// - Number literals
// - String and character literals
// - Single-character operators and punctuation
// - Unknown/Error tokens
Token next_token(Lexer* lexer) {
  MemoryZeroStruct(&lexer->current_token);

  char8 c = *(lexer->current_character);
  if (lexer_at_eof(lexer)) {
    return make_token(lexer, Token_End_Of_File, 1);
  } else if (char8_is_space(c)) {
    return token_from_whitespace(lexer);
  } else if (c == '#') {
    return make_token(lexer, Token_Preprocessor_Hash, 1);  
  } else if ((c == '/' && (peek_character(lexer, 1) == '/' || peek_character(lexer, 1) == '*')) || (c == '*' && peek_character(lexer, 1) == '/')) {
    return token_from_comment(lexer);
  } else if (char8_is_alpha(c) || c == '_') {
    return token_from_identifier_or_keyword(lexer);
  } else if (char8_is_digit(c)) {
    return token_from_number(lexer);
  } else if (c == '"') {
    return token_from_string(lexer);
  } else if (c == '\'') {
    return token_from_character(lexer);
  } else {
    Token token;
    token = token_from_operator(lexer);
    if (token.type != Token_Unknown) {
      return token;
    }
    token = token_from_delimiter(lexer);
    if (token.type != Token_Unknown) {
      return token;
    }
    token = token_from_braces(lexer);
    if (token.type != Token_Unknown) {
      return token;
    }
    return make_token(lexer, Token_Unknown, 1);
  }
}

Token token_from_whitespace(Lexer* lexer) {
  char8 c = *lexer->current_character;
  
  if (c == ' ') {
    return make_token(lexer, Token_Space, 1);
  }

  if (c == '\t') {
    return make_token(lexer, Token_Tab, 1);
  }

  if (c == '\n') {
    lexer->line  += 1;
    lexer->column = 1;
    return make_token(lexer, Token_New_Line, 1);
  }

  if (c == '\r') {
    char8* start = lexer->current_character;

    u32 length = 1;
    if (peek_character(lexer, 1) == '\n') {
      length = 2;
    }

    lexer->line  += 1;
    lexer->column = 1;
    return make_token(lexer, Token_New_Line, length);
  }

  return (Token){.type = Token_Unknown};
}

internal Token token_from_comment(Lexer* lexer) {
  Token token = {0};
  char8* start = lexer->current_character;
  char8 c = *start;

  if (c == '/' && peek_character(lexer, 1) == '/') {
    token.type = Token_Comment_Line;
    token.start_offset = start - lexer->file_start;

    advance(lexer); // consume first '/'
    advance(lexer); // consume second '/'

    while (!lexer_at_eof(lexer)) {
      char8 ch = *(lexer->current_character);
      if (ch == '\n' || ch == '\r') break;
      advance(lexer);
    }

    token.end_offset = lexer->current_character - lexer->file_start;
  }

  else if (c == '/' && peek_character(lexer, 1) == '*') {
    token.type = Token_Comment_Block;
    token.start_offset = start - lexer->file_start;

    advance(lexer); // consume '/'
    advance(lexer); // consume '*'

    while (!lexer_at_eof(lexer)) {
      if (*(lexer->current_character) == '*' && peek_character(lexer, 1) == '/') {
        advance(lexer); // consume '*'
        advance(lexer); // consume '/'
        break;
      }
      advance(lexer);
    }

    token.end_offset = lexer->current_character - lexer->file_start;
  }

  else {
    ERROR_MESSAGE_AND_EXIT("Expected comment token");
  }

  token.line   = lexer->line;
  token.column = lexer->column;
  token.value  = string8_range(lexer->file_start + token.start_offset,
                               lexer->file_start + token.end_offset);

  return token;
}

Token token_from_operator(Lexer* lexer) {
  char8 c = *lexer->current_character;
  char8 next = peek_character(lexer, 1);
  
  switch (c) {
    case '+':
      if (next == '+') {
        return make_token(lexer, Token_Increment, 2);
      } else if (next == '=') {
        return make_token(lexer, Token_Plus_Assign, 2);
      } else {
        return make_token(lexer, Token_Plus, 1);
      }
      
    case '-':
      if (next == '-') {
        return make_token(lexer, Token_Decrement, 2);
      } else if (next == '=') {
        return make_token(lexer, Token_Minus_Assign, 2);
      } else if (next == '>') {
        return make_token(lexer, Token_Arrow, 2);
      } else {
        return make_token(lexer, Token_Minus, 1);
      }
      
    case '*':
      if (next == '=') {
        return make_token(lexer, Token_Multiply_Assign, 2);
      } else {
        return make_token(lexer, Token_Multiply, 1);
      }
      
    case '/':
      if (next == '=') {
        return make_token(lexer, Token_Divide_Assign, 2);
      } else {
        return make_token(lexer, Token_Divide, 1);
      }
      
    case '%':
      if (next == '=') {
        return make_token(lexer, Token_Modulo_Assign, 2);
      } else {
        return make_token(lexer, Token_Modulo, 1);
      }
      
    case '=':
      if (next == '=') {
        return make_token(lexer, Token_Equal, 2);
      } else {
        return make_token(lexer, Token_Assign, 1);
      }
      
    case '!':
      if (next == '=') {
        return make_token(lexer, Token_Not_Equal, 2);
      } else {
        return make_token(lexer, Token_Not, 1);
      }
      
    case '<':
      if (next == '=') {
        return make_token(lexer, Token_Less_Equal, 2);
      } else if (next == '<') {
        if (peek_character(lexer, 2) == '=') {
          return make_token(lexer, Token_Left_Shift_Assign, 3);
        } else {
          return make_token(lexer, Token_Left_Shift, 2);
        }
      } else {
        return make_token(lexer, Token_Less, 1);
      }
      
    case '>':
      if (next == '=') {
        return make_token(lexer, Token_Greater_Equal, 2);
      } else if (next == '>') {
        if (peek_character(lexer, 2) == '=') {
          return make_token(lexer, Token_Right_Shift_Assign, 3);
        } else {
          return make_token(lexer, Token_Right_Shift, 2);
        }
      } else {
        return make_token(lexer, Token_Greater, 1);
      }
      
    case '&':
      if (next == '&') {
        return make_token(lexer, Token_Logical_And, 2);
      } else if (next == '=') {
        return make_token(lexer, Token_Bit_And_Assign, 2);
      } else {
        return make_token(lexer, Token_Bit_And, 1);
      }
      
    case '|':
      if (next == '|') {
        return make_token(lexer, Token_Logical_Or, 2);
      } else if (next == '=') {
        return make_token(lexer, Token_Bit_Or_Assign, 2);
      } else {
        return make_token(lexer, Token_Bit_Or, 1);
      }
      
    case '^':
      if (next == '=') {
        return make_token(lexer, Token_Bit_Xor_Assign, 2);
      } else {
        return make_token(lexer, Token_Bit_Xor, 1);
      }
      
    case '~':
      return make_token(lexer, Token_Bit_Not, 1);
  }
  
  return (Token){.type = Token_Unknown};
}

Token token_from_delimiter(Lexer* lexer) {
  char8 c = *lexer->current_character;
  
  switch (c) {
    case ';':
      return make_token(lexer, Token_Semicolon, 1);
    case ',':
      return make_token(lexer, Token_Comma, 1);
    case '.':
      return make_token(lexer, Token_Dot, 1);
    case ':':
      return make_token(lexer, Token_Colon, 1);
    case '?':
      return make_token(lexer, Token_Question, 1);
  }
  
  return (Token){.type = Token_Unknown};
}

Token token_from_braces(Lexer* lexer) {
  char8 c = *lexer->current_character;
  
  switch (c) {
    case '(':
      return make_token(lexer, Token_Open_Parenthesis, 1);
    case ')':
      return make_token(lexer, Token_Close_Parenthesis, 1);
    case '{':
      return make_token(lexer, Token_Open_Brace, 1);
    case '}':
      return make_token(lexer, Token_Close_Brace, 1);
    case '[':
      return make_token(lexer, Token_Open_Bracket, 1);
    case ']':
      return make_token(lexer, Token_Close_Bracket, 1);
  }
  
  return (Token){.type = Token_Unknown};
}

Token token_from_identifier_or_keyword(Lexer* lexer) {
  char8* start = lexer->current_character;
  char8 c = *start;
  u32 len = 0;
  
  while (char8_is_alphanum(c) || c == '_') {
    c = peek_character(lexer, len++);
  }
  len -= 1;
  
  Token token = make_token_range(lexer, Token_Identifier, start, start + len);
  Token_Type keyword_type = is_token_keyword(token);
  if (keyword_type != Token_Identifier) {
    token.type = keyword_type;
  }
  
  return token;
}

Token token_from_number(Lexer* lexer) {
  
  char8* start = lexer->current_character;
  char8 c      = *start;
  Token_Type token_type = Token_Int_Literal;
  
  u32 len = 0;

  // Handle hex numbers
  if (c == '0' && (peek_character(lexer, 1) == 'x' || peek_character(lexer, 1) == 'X')) {
    len += 2;
    c = peek_character(lexer, len);

    while (char8_is_hex_digit(c)) {
      c = peek_character(lexer, len++);
    }
    token_type = Token_Hex_Literal;
  } else {
    // Handle decimal numbers
    while (char8_is_digit(c)) {
      c = peek_character(lexer, len++);
    }
    
    // Handle floating point
    if (c == '.') {
      c = peek_character(lexer, len++);
      while (char8_is_digit(c)) {
        c = peek_character(lexer, len++);
      }
      token_type = Token_Float_Literal;
    }
    
    // Handle scientific notation
    if (c == 'e' || c == 'E') {
      c = peek_character(lexer, len++);
      if (c == '+' || c == '-') {
        c = peek_character(lexer, len++);
      }
      while (char8_is_digit(c)) {
        c = peek_character(lexer, len++);
      }
      token_type = Token_Float_Literal;
    }
  }
  
  // Handle suffixes
  char8 suffix = c;
  if (suffix == 'f' || suffix == 'F') {
    c = peek_character(lexer, len++);
    token_type = Token_Float_Literal;
  } else if (suffix == 'l' || suffix == 'L' || suffix == 'u' || suffix == 'U') {
    c = peek_character(lexer, len++);
    // Handle UL, LU combinations
    char8 next_suffix = c;
    if ((suffix == 'u' || suffix == 'U') && (next_suffix == 'l' || next_suffix == 'L')) {
      c = peek_character(lexer, len++);
    } else if ((suffix == 'l' || suffix == 'L') && (next_suffix == 'u' || next_suffix == 'U')) {
      c = peek_character(lexer, len++);
    }
  }
  
  len -= 1; // len will go to the next non valid character, which is a different token
  return make_token_range(lexer, token_type, start, start + len);
}


Token token_from_string(Lexer* lexer) {
  advance(lexer); // + 1 to skip opening quote
  char8* start = lexer->current_character;
  char8 c = *start;
  u32 len = 0;

  c = peek_character(lexer, len + 1); // pre increment to skip open quote
  
  while (!lexer_at_eof(lexer) && c != '"') {
    if (c == '\\') {
      c = peek_character(lexer, len++);
      if (!lexer_at_eof(lexer)) {
        c = peek_character(lexer, len++);
      }
    } else {
      c = peek_character(lexer, len++);
    }
  }
  
  len -= 1; // We don't want the closing quote
  advance(lexer);
  return make_token_range(lexer, Token_String_Literal, start, start + len);
}

Token token_from_character(Lexer* lexer) {
  advance(lexer);
  char8* start = lexer->current_character;

  char8 c = *start;
  u32 len = 0;

  c = peek_character(lexer, len++);
  
  if (!lexer_at_eof(lexer)) {
    if (c == '\\') {
      c = peek_character(lexer, len++);
      if (!lexer_at_eof(lexer)) {
        c = peek_character(lexer, len++);
      }
    } else {
      c = peek_character(lexer, len++);
    }
  }
  
  len -= 1; // We don't want the closing quote
  advance(lexer);
  return make_token_range(lexer, Token_Char_Literal, start, start + len);
}

Token make_token_range(Lexer* lexer, Token_Type type, char8* start, char8* end) {
  lexer->current_token.type         = type;
  lexer->current_token.value.str    = start;
  lexer->current_token.value.size   = (u32)(end - start);
  lexer->current_token.start_offset = offset_of_character(lexer, start);
  lexer->current_token.end_offset   = offset_of_character(lexer, end);

  advance_by(lexer, (u32)(end - start));

  return lexer->current_token;
}

Token make_token(Lexer* lexer, Token_Type type, u32 length) {
  lexer->current_token.type         = type;
  lexer->current_token.value.str    = lexer->current_character;
  lexer->current_token.value.size   = length;
  lexer->current_token.start_offset = offset_of_character(lexer, lexer->current_character);
  lexer->current_token.end_offset   = offset_of_character(lexer, lexer->current_character + length);

  advance_by(lexer, length);

  return lexer->current_token;
}

Token_Type is_token_keyword(Token identifier_token) {
  String8 value = identifier_token.value;
  
  if (string8_equal(value, Str8("return")))    return Token_Return;
  if (string8_equal(value, Str8("if")))        return Token_If;
  if (string8_equal(value, Str8("else")))      return Token_Else;
  if (string8_equal(value, Str8("while")))     return Token_While;
  if (string8_equal(value, Str8("for")))       return Token_For;
  if (string8_equal(value, Str8("break")))     return Token_Break;
  if (string8_equal(value, Str8("continue")))  return Token_Continue;
  if (string8_equal(value, Str8("struct")))    return Token_Struct;
  if (string8_equal(value, Str8("union")))     return Token_Union;
  if (string8_equal(value, Str8("enum")))      return Token_Enum;
  if (string8_equal(value, Str8("typedef")))   return Token_Typedef;
  if (string8_equal(value, Str8("static")))    return Token_Static;
  if (string8_equal(value, Str8("void")))      return Token_Void;
  if (string8_equal(value, Str8("int")))       return Token_Int;
  if (string8_equal(value, Str8("char")))      return Token_Char;
  if (string8_equal(value, Str8("float")))     return Token_Float;
  if (string8_equal(value, Str8("double")))    return Token_Double;
  if (string8_equal(value, Str8("unsigned")))  return Token_Unsigned;
  if (string8_equal(value, Str8("signed")))    return Token_Signed;
  if (string8_equal(value, Str8("const")))     return Token_Const;
  if (string8_equal(value, Str8("extern")))    return Token_Extern;
  if (string8_equal(value, Str8("switch")))    return Token_Switch;
  if (string8_equal(value, Str8("case")))      return Token_Case;
  if (string8_equal(value, Str8("default")))   return Token_Default;
  if (string8_equal(value, Str8("sizeof")))    return Token_Sizeof;
  if (string8_equal(value, Str8("inline")))    return Token_Inline;
  if (string8_equal(value, Str8("do")))        return Token_Do;
  if (string8_equal(value, Str8("goto")))      return Token_Goto;
  if (string8_equal(value, Str8("restrict")))  return Token_Restrict;
  if (string8_equal(value, Str8("volatile")))  return Token_Volatile;
  if (string8_equal(value, Str8("register")))  return Token_Register;
    
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

void advance(Lexer* lexer) {
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
    advance(lexer);
  }
}

b32 is_token_whitespace(Token token) {
  b32 result = (token.type == Token_Space ||
                token.type == Token_Tab   ||
                token.type == Token_New_Line);
  return result;
}

void token_print(Token token) {
  // Safety: check enum range
  if (token.type >= Token_Count) {
    printf("xx Token_Type out of range: %d\n", token.type);
    return;
  }

  // Print token type
  printf("%s: ", token_type_names[token.type]);
  for (u32 i = strlen(token_type_names[token.type]); i < 26; i += 1) {
    printf(" ");
  }

  Assert(token.start_offset + token.value.size == token.end_offset);

  // Print token value if it has one (non-empty string)
  if (token.value.size > 0 && token.value.str) {
    if (token.type == Token_New_Line) {
      printf("Token value: '\\n'");
    } else {
      printf("Token value: '%.*s'", (s32)token.value.size, token.value.str);
    }
    for (u32 i = token.value.size; i < 16; i += 1) {
      printf(" ");
    }
    printf("StartEnd: [%d, %d]", token.start_offset, token.end_offset);
  }
  printf("\n");

  return;
}
