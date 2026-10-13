#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static int
is_separator(char c)
{
  return strchr(" -\r\t\n./,", c) != 0;
}

static void
process_number(long number, int has_number)
{
  if (has_number && (number % 5 == 0 || number % 6 == 0)) {
    printf("%ld\n", number);
  }
}

static void
process_fd(int fd)
{
  char c;
  long number = 0;
  int has_number = 0;

  while (read(fd, &c, 1) == 1) {

    if (c >= '0' && c <= '9') {
      number = number * 10 + (c - '0');
      has_number = 1;
    } else if (is_separator(c)) {
      process_number(number, has_number);

      number = 0;
      has_number = 0;
    } else {
      number = 0;
      has_number = 0;
    }
  }

  process_number(number, has_number);
}

int
main(int argc, char *argv[])
{
  int fd;

  if (argc == 1) {
    process_fd(0);
    exit(0);
  }

  for (int i = 1; i < argc; i++) {
    fd = open(argv[i], O_RDONLY);

    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      continue;
    }

    process_fd(fd);

    if (close(fd) < 0) {
      fprintf(2, "sixfive: cannot close %s\n", argv[i]);
    }
  }

  exit(0);
}