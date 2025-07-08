int main() {
  int arr[5] = {1, 2, 3, 4, 5};
  int* p = arr;
  int x = *(p + 2);
  int y = p[3];

  int matrix[2][3] = {{1, 2, 3}, {4, 5, 6}};
  int z = matrix[1][2];

  int (*fp)(int);
  int f(int a) { return a * 2; }
  fp = f;
  int r = fp(5);

  int* ptrs[3];
  ptrs[0] = &x;

  return arr[0] + matrix[0][0] + r;
}
