#include <dirent.h>
#include <getopt.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void search_file(const char *searchpath, const char *filename, bool recursive,
                 bool caseSensitive) {
  DIR *dir = opendir(searchpath);
  if (!dir) {
    perror("failed to open Dirrectory");
    return;
  }

  struct dirent *dirrectoryentry;
  while ((dirrectoryentry = readdir(dir)) != NULL) {
    if (strcmp(dirrectoryentry->d_name, ".") == 0 ||
        strcmp(dirrectoryentry->d_name, "..") == 0) {
      continue;
    }

    int match;
    if (caseSensitive) {
      match = strcmp(dirrectoryentry->d_name, filename) == 0;
    } else {
      match = strcasecmp(dirrectoryentry->d_name, filename) == 0;
    }

    if (match) {
      char fullpath[PATH_MAX];
      snprintf(fullpath, sizeof(fullpath), "%s%s", searchpath,
               dirrectoryentry->d_name);

      printf("%d: %s: %s\n", getpid(), filename, fullpath);
      fflush(stdout);
    }
  }

  closedir(dir);
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
