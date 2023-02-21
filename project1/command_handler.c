#include "command_handler.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *path;
int num_of_paths = 0;
const int ERROR = 1;
const int SUCCESS = 0;
const char *const ERROR_MESSAGE = "An error has occurred\n";

void init_path() {
  // allocate memory for "/bin" plus null terminator
  path = (char *)malloc(sizeof(char) * 32);
  strcpy(path, "/bin");
}

bool is_builtin(TokenList tokens) {
  if (strcmp(tokens.tokens[0], "path") == 0 ||
      strcmp(tokens.tokens[0], "cd") == 0 ||
      strcmp(tokens.tokens[0], "exit") == 0) {
    return true;
  } else {
    return false;
  }
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
      WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
    }
  } else if (strcmp(tokens.tokens[0], "cd") == 0) {
    if (tokens.num_tokens == 1) {
      WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
    } else {
      int result = chdir(tokens.tokens[1]);
      if (result != SUCCESS) {
        WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
      }
    }
  } else if (strcmp(tokens.tokens[0], "path") == 0) {
    if (tokens.num_tokens == 1) {
      set_path(&path, "\0");
    } else if (tokens.num_tokens == 2) {
      set_path(&path, tokens.tokens[1]);
    } else {
      WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
    }
  } else {
    WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
  }
}

void execute_command(TokenList tokens) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  } else if (pid == 0) {
    if (path == NULL) {
      WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
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
          WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
          exit(EXIT_FAILURE);
        }
      }
      token = strsep(&p, delim);
    }
    WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
    exit(EXIT_FAILURE);
  } else {
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
    }
  }
}
