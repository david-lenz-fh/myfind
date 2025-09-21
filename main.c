#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void search_file(const char *searchpath, const char *filename, bool recursive,
                 bool caseSensitive) {
  printf("%d: searching for %s in %s\n", getpid(), filename, searchpath);
  fflush(stdout);
}

int main(int argc, char *argv[]) {
  int c;
  bool searchRecursive = false;
  bool caseSensitive = true;

  int currentArgument = 1;
  while ((c = getopt(argc, argv, "Ri")) != -1) {
    currentArgument++;
    switch (c) {
    case 'R':
      searchRecursive = true;
      printf("\nRekursiv");
      break;
    case 'i':
      caseSensitive = false;
      printf("\ncase insensitive");
      break;
    case '?':
      break;
    }
  }
  char *searchpath = argv[optind];
  int argremain = argc - optind;
  if (argremain <= 1) {
    return 1;
  }

  for (int i = optind + 1; i < argc; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      search_file(searchpath, argv[i], searchRecursive, caseSensitive);
      exit(0);
    }
  }

  while (wait(NULL) > 0)
    ;

  return 0;
}
