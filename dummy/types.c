typedef int MyInt;
MyInt a;

struct Point {
  int x, y;
};

union Value {
  int i;
  float f;
};

enum Color {
  RED, GREEN = 5, BLUE
};

struct SelfRef {
  struct SelfRef* next;
};

struct {
  int a : 3;
  int b : 5;
} bitfield;
