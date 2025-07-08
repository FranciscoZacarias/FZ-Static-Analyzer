int main() {
  int x = 0;

  if (x == 0) {
    x = 1;
  } else {
    x = 2;
  }

  while (x < 10) {
    x++;
  }

  do {
    x--;
  } while (x > 0);

  for (int i = 0; i < 10; i++) {
    x += i;
  }

  switch (x) {
    case 1: x = 100; break;
    case 2: x = 200; break;
    default: x = 300;
  }

  label:
  if (x == 300) goto label;

  return x;
}
