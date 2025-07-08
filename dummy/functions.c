void f1() {
}

int f2(int a, float b) {
  return a + (int)b;
}

void f3(int a, ...) {
}

int f4();

int f4() {
  return 123;
}

int factorial(int n) {
  if (n <= 1) return 1;
  return n * factorial(n - 1);
}
