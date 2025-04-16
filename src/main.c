#include "main.h"

Arena* GeneralArena;
File_List ProjectFiles;

void application_init() {
  GeneralArena = arena_init();

  u32 flags = FileFlag_WhiteList | FileFlag_CFiles | FileFlag_HFiles | FileFlag_Dirs;
  ProjectFiles = file_get_all_files_in_path_recursively(GeneralArena, Str8("D:\\work\\project_checker"), flags);
  
  File_Node* current_file;
  current_file = ProjectFiles.first;
  do {
    println_string(current_file->value.path);
    current_file = current_file->next;
  } while (current_file->next);

  printf("Nodes: %lld\n", ProjectFiles.node_count);
  println_string(ProjectFiles.first->value.path);
  println_string(ProjectFiles.last->value.path);
}

void application_tick() {
}