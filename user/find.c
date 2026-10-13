#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "user/user.h"

static char *
fmtname(char *path)
{
  char *p;
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

static void
runexec(char **command, int command_count, char *file)
{
  char *args[MAXARG];
  int i;
  int pid;

  for (i = 0; i < command_count; i++)
    args[i] = command[i];
  args[i++] = file;
  args[i] = 0;

  pid = fork();
  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }
  if (pid == 0) {
    exec(args[0], args);
    fprintf(2, "find: exec %s failed\n", args[0]);
    exit(1);
  }
  wait(0);
}

static void
find(char *path, char *name, char **command, int command_count)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type == T_FILE) {
    if (strcmp(name, fmtname(path)) == 0) {
      if (command_count)
        runexec(command, command_count, path);
      else
        printf("%s\n", path);
    }
  } else if (st.type == T_DIR) {
    if (strcmp(name, fmtname(path)) == 0) {
      if (command_count)
        runexec(command, command_count, path);
      else
        printf("%s\n", path);
    }
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      printf("find: path too long\n");
      close(fd);
      return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
          strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, name, command, command_count);
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "usage: find dir name [-exec cmd...]\n");
    exit(1);
  }
  if (argc > 3 && strcmp(argv[3], "-exec") == 0)
    find(argv[1], argv[2], argv + 4, argc - 4);
  else
    find(argv[1], argv[2], 0, 0);
  exit(0);
}
