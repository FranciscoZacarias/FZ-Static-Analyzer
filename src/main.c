
#define DEBUG 1
#define PRINT_TOKENS 1
#define FZ_ENABLE_ASSERT 1 
#include "main.h"

#define TEST_FILE Str8("top_level_constructs.c")

void entry_point(Command_Line command_line) {
  Arena* arena = arena_init();
  win32_enable_console(true);
  
  String8 pwd = path_get_working_directory();
  String8 dir = path_get_current_directory_name(pwd);
  if (string8_equal(dir, Str8("build"))) {
    pwd = path_dirname(pwd); // move back if in build
  }
  pwd = path_join(arena, pwd, Str8("dummy"));
  String8_List files = file_get_all_file_paths_recursively(arena, pwd);

  for (String8_Node* node = files.first; node != NULL; node = node->next) {
    String8 path = node->value;
    b32 is_dot_c = file_has_extension(path, Str8(".c"));
    b32 is_dot_h = file_has_extension(path, Str8(".h"));
    if (!is_dot_c && !is_dot_h) {
      continue;
    }

    u64 index = 0;
    if (string8_find_last(path, Str8("\\"), &index) && index+1 <= path.size-1) {
      String8 file_string8 = string8_slice(path, index+1, path.size);

      if (!string8_equal(file_string8, TEST_FILE)) {
        continue;
      }

      printf("\n==== New File ====\n> ");
      printf_color(Terminal_Color_Bright_Green, cstring_from_string8(arena, file_string8));
      printf("\n");
    } else {
      printf("Unable to parse cstring from: "); string8_printf(path); printf("\n");
      continue;
    }

    Lexer lexer;
    Token_Array tokens = load_all_tokens(&lexer, path);
    
    Parser parser;
#if DEBUG
    parser.file = &lexer.file;
#endif

    AST_Node* ast = parse_ast(&parser, tokens);

    printf("\n");
    print_ast(&parser, &lexer, true, true);

	  printf("\n------------------\n");
  }

  system("pause");
}