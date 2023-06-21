#include <stdlib.h>

unsigned long fibo(unsigned int n) {
  unsigned long a;
  unsigned long b;

  a = 0;
  b = 1;

  while (n-- > 0) {
    b += a;
    a = b - a;
  }

  return a;
}

unsigned int str2uint(char* str) {
  unsigned long n;
  unsigned int i;

  n = 0;
  i = 0;
  while (str[i] != '\0') {
    n *= 10;
    n += str[i] - '0';
    i++;
  }

  return n;
}

long main(int argc, char** argv) {
  unsigned long a;
  unsigned int n;

  if (argc < 2) {
    n = 42;
    a = fibo(n);
    printf("%lu\n", a);
    return a;
  }
  else if (argc < 3) {
    n = str2uint(argv[1]);
    a = fibo(n);
    printf("%lu\n", a);
    return a;
  }
  else {
    a = str2uint(argv[1]);
    n = 0;
    for (; n <= a; n++) {
      printf("%lu\n", fibo(n));
    }
  }
  return 0;
}
