
#define DEBUG 1
#define PRINT_TOKENS 1
#include "main.h"

#define WORKSPACE_PATH Str8("C:\\Personal\\FZ-Static-Analyzer\\dummy")

void entry_point() {
  Arena* arena = arena_init();
  win32_enable_console(true);
  
  File_List files = file_get_all_files_in_path_recursively(arena, WORKSPACE_PATH, (FileFlag_WhiteList | FileFlag_CFiles | FileFlag_HFiles | FileFlag_Dirs));
  File_Node* current_file = files.first;

  while (current_file != NULL) {


    u64 index = 0;
    if (string8_find_last(current_file->value.path, Str8("\\"), &index) && index+1 <= current_file->value.path.size-1) {
      String8 file_string8 = string8_slice(current_file->value.path, index+1, current_file->value.path.size);

      if (!string8_equal(file_string8, Str8("expressions.c"))) {
        current_file = current_file->next;
        continue;
      }

      printf("\n==== New File ====\n> ");
      printf_color(Terminal_Color_Bright_Green, cstring_from_string8(arena, file_string8));
      printf("\n");
    } else {
      printf("Unable to parse cstring from: "); string8_printf(current_file->value.path); printf("\n");
      current_file = current_file->next;
      continue;
    }

    Lexer lexer;
    lexer_init(&lexer, current_file->value.path);

    Parser parser;
    parser_init(&parser, &lexer);

    parser_parse_file(&parser);
    printf("\n");
    parser_print_ast(&parser, true, true);

	  printf("\n------------------\n");

    current_file = current_file->next;
  }

  system("pause");
}