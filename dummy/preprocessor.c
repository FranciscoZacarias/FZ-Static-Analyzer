#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PI 3.14159
#define STR(x) #x
#define CONCAT(a, b) a##b

#include <stdio.h>
#include "dummy_header.h"

int main() {
#if defined(PI)
  double pi = PI;
#endif

#ifndef DEBUG
  printf("Release mode\n");
#else
  printf("Debug mode\n");
#endif

#undef PI

  int ab = 10;
  int x = CONCAT(a, b);

  printf("%s = %d\n", STR(x), x);
  return 0;
}
