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
    perror("failed to open Directory");
    return;
  }

  struct dirent *directoryentry;
  while ((directoryentry = readdir(dir)) != NULL) {
    if (strcmp(directoryentry->d_name, ".") == 0 ||
        strcmp(directoryentry->d_name, "..") == 0) {
      continue;
    }

    int match;
    if (caseSensitive) {
      match = strcmp(directoryentry->d_name, filename) == 0;
    } else {
      match = strcasecmp(directoryentry->d_name, filename) == 0;
    }

    if (match) {
      char fullpath[PATH_MAX];
      snprintf(fullpath, sizeof(fullpath), "%s/%s", searchpath,
               directoryentry->d_name);

      printf("%d: %s: %s\n", getpid(), filename, fullpath);
      fflush(stdout);
    }

    if (!recursive)
      continue;
    if (directoryentry->d_type == DT_DIR) {
      char newsearchpath[PATH_MAX];
      snprintf(newsearchpath, sizeof(newsearchpath), "%s/%s", searchpath,
               directoryentry->d_name);
      search_file(newsearchpath, filename, recursive, caseSensitive);
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
      printf("Rekursiv\n");
      break;
    case 'i':
      caseSensitive = false;
      printf("Case insensitive\n");
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
