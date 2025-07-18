#include <stdio.h>
# include "local.h"

int main() {
  char a = 'a';
  char s = ' ';
  char q = '\'';
  const char* s1 = "Hello";
  const char* s2 = "World";
  const char* escaped = "Line1\nLine2\tTabbed";
  const wchar_t* ws = L"Wide string";

  printf("%s\n", s1);
  printf("%s\n", s2);
  printf("%s\n", s3);
  printf("%s\n", escaped);
  return 0;
}
