#include "kernel/types.h"
#include "user/user.h"

void memdump(char *, char *, int);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    int a[2] = {61810, 2026};
    printf("Example 1:\n");
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct example {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } value;
    value.ptr = "hello";
    value.num1 = 1819438967;
    value.num2 = 100;
    value.byte = 'z';
    strcpy(value.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&value, sizeof(value));
    printf("Example 5:\n");
    memdump("sccccc", (char *)&value, sizeof(value));
  } else if (argc == 2) {
    char data[512];
    int n = 0;
    memset(data, 0, sizeof(data));
    while (n < sizeof(data)) {
      int readn = read(0, data + n, sizeof(data) - n);
      if (readn <= 0)
        break;
      n += readn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
  int off = 0;

  for (char *f = fmt; *f; f++) {
    char c = *f;
    if (c == 'i') {
      if (off + 4 > len) {
        printf("memdump: not enough data for '%c'\n", c);
        return;
      }
      int value;
      memmove(&value, data + off, 4);
      printf("%d\n", value);
      off += 4;
    } else if (c == 'p') {
      if (off + 8 > len) {
        printf("memdump: not enough data for '%c'\n", c);
        return;
      }
      uint64 value;
      memmove(&value, data + off, 8);
      printf("%lx\n", value);
      off += 8;
    } else if (c == 'h') {
      if (off + 2 > len) {
        printf("memdump: not enough data for '%c'\n", c);
        return;
      }
      short value;
      memmove(&value, data + off, 2);
      printf("%d\n", value);
      off += 2;
    } else if (c == 'c') {
      if (off + 1 > len) {
        printf("memdump: not enough data for '%c'\n", c);
        return;
      }
      printf("%c\n", data[off++]);
    } else if (c == 's') {
      if (off + 8 > len) {
        printf("memdump: not enough data for '%c'\n", c);
        return;
      }
      uint64 value;
      memmove(&value, data + off, 8);
      printf("%s\n", (char *)value);
      off += 8;
    } else if (c == 'S') {
      int i = off;
      while (i < len && data[i] != '\0')
        printf("%c", data[i++]);
      printf("\n");
      off = len;
    }
  }
}
