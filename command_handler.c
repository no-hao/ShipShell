#include "command_handler.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *path;
const int ERROR = 1;
const int SUCCESS = 0;

void init_path() {
  // allocate memory for "/bin" plus null terminator
  path = (char *)malloc(sizeof(char) * 32);
  strcpy(path, "/bin");
}

bool is_builtin(TokenList tokens) {
  return (strcmp(tokens.tokens[0], "path") == 0 ||
          strcmp(tokens.tokens[0], "cd") == 0 ||
          strcmp(tokens.tokens[0], "exit") == 0);
}

void set_path(char **path, const char *new_path) {
  if (*path != NULL) {
    free(*path);
    *path = NULL;
  }

  if (new_path == NULL || strcmp(new_path, "") == 0) {
    *path = NULL;
  } else {
    *path = (char *)malloc(sizeof(char) * (strlen(new_path) + 1));
    strcpy(*path, new_path);
  }
}

void execute_builtin(TokenList tokens) {
  if (strcmp(tokens.tokens[0], "exit") == 0) {
    if (tokens.num_tokens == 1) {
      exit(SUCCESS);
    } else {
      print_error();
    }
  } else if (strcmp(tokens.tokens[0], "cd") == 0) {
    if (tokens.num_tokens == 1) {
      print_error();
    } else {
      int result = chdir(tokens.tokens[1]);
      if (result != SUCCESS) {
        print_error();
      }
    }
  } else if (strcmp(tokens.tokens[0], "path") == 0) {
    if (tokens.num_tokens == 1) {
      set_path(&path, "\0");
    } else {
      // Concatenate all the path tokens into a single string separated by
      // colons
      char *new_path_str = NULL;
      for (int i = 1; i < tokens.num_tokens; i++) {
        if (new_path_str == NULL) {
          new_path_str = strdup(tokens.tokens[i]);
        } else {
          char *temp = strdup(new_path_str);
          free(new_path_str);
          new_path_str = (char *)malloc(
              sizeof(char) * (strlen(temp) + strlen(tokens.tokens[i]) + 2));
          sprintf(new_path_str, "%s:%s", temp, tokens.tokens[i]);
          free(temp);
        }
      }

      // Update the path variable with the concatenated path string
      set_path(&path, new_path_str);

      // Free the memory used by the concatenated path string
      free(new_path_str);
    }
  } else {
    print_error();
  }
}

void execute_command(TokenList tokens) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  } else if (pid == 0) {
    if (path == NULL) {
      print_error();
      exit(EXIT_FAILURE);
    }

    char full_path[255];
    char *p = path;
    char *delim = ":";
    char *token = strsep(&p, delim);
    while (token != NULL) {
      sprintf(full_path, "%s/%s", token, tokens.tokens[0]);
      if (access(full_path, X_OK) == 0) {
        if (execv(full_path, tokens.tokens) == -1) {
          // print_error();
          exit(EXIT_FAILURE);
        }
      }
      token = strsep(&p, delim);
    }
    print_error();
    exit(EXIT_FAILURE);
  } else {
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
    }
  }
}
