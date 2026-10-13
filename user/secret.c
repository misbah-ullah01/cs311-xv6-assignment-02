#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define DATASIZE (9 * 4096)

int
main(int argc, char *argv[])
{
  char *data;
  unsigned long align;

  if (argc != 2) {
    printf("Usage: secret the-secret\n");
    exit(1);
  }
  data = sbrk(DATASIZE);
  align = ((unsigned long)data) % 4096;
  if (align != 0)
    data += 4096 - align;
  if (strlen(argv[1]) > 0)
    data += 32 + (unsigned int)(argv[1][0] & 0xff);
  strcpy(data, "Here it is: ");
  strcpy(data + strlen("Here it is: "), argv[1]);
  exit(0);
}
