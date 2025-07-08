int main() {
  int a = 1 + 2 * 3;
  int b = (4 + 5) * 6;
  int c = -a + ~b;
  int d = a < b && b != c || c >= d;
  int e = a ? b : c;
  int* p = &a;
  int f = sizeof(int);
  int g = (int)3.14;
  return 0;
}