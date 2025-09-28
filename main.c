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
                 bool caseSensitive, int pipefd) {
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

      char msg[PATH_MAX + 128];
      snprintf(msg, sizeof(msg), "%d: %s: %s\n", getpid(), filename, fullpath);

      write(pipefd, msg, strlen(msg));
    }

    if (!recursive)
      continue;
    if (directoryentry->d_type == DT_DIR) {
      char newsearchpath[PATH_MAX];
      snprintf(newsearchpath, sizeof(newsearchpath), "%s/%s", searchpath,
               directoryentry->d_name);
      search_file(newsearchpath, filename, recursive, caseSensitive, pipefd);
    }
  }

  closedir(dir);
}

int main(int argc, char *argv[]) {
  int c;
  bool searchRecursive = false;
  bool caseSensitive = true;

  while ((c = getopt(argc, argv, "Ri")) != -1) {
    switch (c) {
    case 'R':
      searchRecursive = true;
      break;
    case 'i':
      caseSensitive = false;
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

  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipefailed");
    return 1;
  }

  for (int i = optind + 1; i < argc; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      close(fd[0]);
      search_file(searchpath, argv[i], searchRecursive, caseSensitive, fd[1]);
      close(fd[1]);
      exit(0);
    }
  }

  close(fd[1]);
  char buffer[1024];
  ssize_t n;
  while ((n = read(fd[0], buffer, sizeof(buffer) - 1)) > 0) {
    buffer[n] = '\0';
    printf("%s", buffer);
    fflush(stdout);
  }
  close(fd[0]);

  while (wait(NULL) > 0)
    ;

  return 0;
}
