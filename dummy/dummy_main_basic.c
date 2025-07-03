// This is a line comment
/*
   This is a block comment
*/

#include <stdio.h>
#define PI 3.14
#ifdef DEBUG
  #define LOG(x) printf(x)
#else
  #define LOG(x)
#endif

#pragma once

// Keywords + literals
static const double g_value = 10.5;
extern unsigned int counter = 0;
volatile int flag = 1;
auto char_val = 'a';
register float temp = 1.0f;
restrict double* ptr = 0;

typedef struct Point {
  int x;
  int y;
} Point;

union Data {
  int i;
  float f;
};

enum Colors {
  RED,
  GREEN,
  BLUE = 10,
};

inline int max(int a, int b) {
  return (a > b) ? a : b;
}

void do_work(int n) {
  int i = 0;
  do {
    i++;
    if (i == n) {
      break;
    } else if (i % 2 == 0) {
      continue;
    }
  } while (i < n);
}

int main() {
  // Arithmetic + assignment
  int a = 5;
  int b = 2;
  a += b;
  b *= 3;
  a /= 2;
  a %= 3;
  a <<= 1;
  b >>= 1;
  a ^= b;
  a |= b;
  a &= b;

  // Increment/decrement
  a++;
  b--;

  // Logical/bitwise
  if (a > 0 && b < 5 || a == b) {
    a = a + b;
  }

  if (!(a != b)) {
    a = ~b;
  }

  // Ternary + colon + question
  int res = (a < b) ? a : b;

  // Struct + Arrow + Dot
  Point p = {1, 2};
  Point* ptr_p = &p;
  int val = ptr_p->x + p.y;

  // Array
  int array[3] = {0, 1, 2};
  int elem = array[1];

  // Sizeof
  int size = sizeof(int);

  // Goto
  goto label;
  label:
  return 0;
}
