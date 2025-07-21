// test.c

#include <stdio.h>             // System include
# include "local_include.c"    // Local include
#define PI 3.14                // Preprocessor macro

_Static_assert(sizeof(int) == 4, "int must be 4 bytes");  // Static assert

typedef int MyInt;            // Typedef

struct Vec2 {                 // Struct definition
  float x;
  float y;
};

union Data {
  int   i;
  float f;
};

enum Color {
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
};

int global_var = 42;          // Global variable declaration

const float threshold = 0.01; // Global const

static char* msg;             // Static global variable

void helper(void);            // Function prototype

int add(int a, int b) {       // Function definition
  return a + b;
}

static void log_result(int result) {  // Static function definition
  printf("Result: %d\n", result);
}

int main(void) {              // Main function
  int result = add(1, 2);
  log_result(result);
  return 0;
}

// Empty declarations
;

;
