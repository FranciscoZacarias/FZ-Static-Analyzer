int (*func_ptr_array[3])(int, float);

int (*(*fp_returning_func_ptr())[])(double) {
  return 0;
}

int main() {
  int (*f1)(int) = 0;
  int (*f2[2])(int) = {0};

  int a[2][3][4];

  struct {
    int (*cb)(int);
    void (*handler[3])(void);
  } s;

  return 0;
}
