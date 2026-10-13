#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define LEN 65536

static int
isalnum_check(char c)
{
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z');
}

int
main(void)
{
  char *mem = sbrk(LEN);

  if (mem == (char *)-1)
    exit(1);
  for (int i = 0; i < LEN; i++) {
    if (isalnum_check(mem[i])) {
      int j = i;
      while (j < LEN && isalnum_check(mem[j]))
        j++;
      if (j - i >= 4) {
        for (int k = i; k < j; k++)
          printf("%c", mem[k]);
        printf("\n");
      }
      i = j;
    }
  }
  exit(0);
}
