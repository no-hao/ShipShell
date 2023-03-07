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

// Initialize the path
Path *init_path() {
  Path *path = malloc(sizeof(Path));
  path->dirs = malloc(sizeof(char *));
  path->dirs[0] = strdup("/bin");
  path->num_dirs = 1;
  return path;
}

void free_path(Path *path) {
  for (int i = 0; i < path->num_dirs; i++) {
    free(path->dirs[i]);
  }
  free(path->dirs);
  free(path);
}

void set_path(Path *path, const char *new_path) {
  // Free the old directories
  for (int i = 0; i < path->num_dirs; i++) {
    free(path->dirs[i]);
  }
  free(path->dirs);

  // Make a non-const copy of the input string
  char *new_path_copy = strdup(new_path);

  // Split the new path into directories
  char *token = strtok(new_path_copy, ":");
  int num_dirs = 0;
  while (token != NULL) {
    num_dirs++;
    token = strtok(NULL, ":");
  }

  // Allocate space for the new directories
  path->dirs = malloc(sizeof(char *) * num_dirs);

  // Copy the new directories
  int i = 0;
  token = strtok(new_path_copy, ":");
  while (token != NULL) {
    path->dirs[i] = strdup(token);
    i++;
    token = strtok(NULL, ":");
  }

  path->num_dirs = num_dirs;

  // Free the copy of the input string
  free(new_path_copy);
}

void exit_command(Command command) {
  if (command.num_args == 1) {
    exit(SUCCESS);
  } else {
    WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
  }
}

void cd_command(Command command) {
  if (command.num_args == 1) {
    WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
  } else {
    int result = chdir(command.args[1]);
    if (result != SUCCESS) {
      WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
    }
  }
}

void path_command(Command command, Path *path) {
  if (command.num_args == 1) {
    set_path(path, "");
  } else if (command.num_args == 2) {
    set_path(path, command.args[1]);
  } else {
    WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
  }
}

bool is_builtin(Command command) {
  return strcmp(command.args[0], "path") == 0 ||
         strcmp(command.args[0], "cd") == 0 ||
         strcmp(command.args[0], "exit") == 0;
}

void execute_builtin(Command command, Path *path) {
  if (strcmp(command.args[0], "exit") == 0) {
    exit_command(command);
  } else if (strcmp(command.args[0], "cd") == 0) {
    cd_command(command);
  } else if (strcmp(command.args[0], "path") == 0) {
    path_command(command, path);
  } else {
    WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
  }
}

void execute_command(Command command, Path *path) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  } else if (pid == 0) {
    if (path == NULL) {
      WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
      exit(EXIT_FAILURE);
    }

    char full_path[255];
    for (int i = 0; i < path->num_dirs; i++) {
      sprintf(full_path, "%s/%s", path->dirs[i], command.args[0]);
      if (access(full_path, X_OK) == 0) {
        if (execv(full_path, command.args) == -1) {
          WRITE_ERROR_MESSAGE(ERROR_MESSAGE);
          exit(EXIT_FAILURE);
        }
      }
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
