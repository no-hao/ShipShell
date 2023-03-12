#include "command_handler.h"
#include "error_handler.h"
#include <fcntl.h>
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

void print_commands(Command command) {
  for (int i = 0; i < command.num_args; i++) {
    printf("Command[%d]: %s\n", i, command.args[i]);
  }
}

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
  char *token, *str_ptr;
  int num_dirs = 0;

  str_ptr = new_path_copy;
  while ((token = strsep(&str_ptr, " ")) != NULL) {
    if (*token != '\0') {
      num_dirs++;
    }
  }

  // Allocate space for the new directories
  path->dirs = malloc(sizeof(char *) * num_dirs);

  // Copy the new directories
  int i = 0;
  str_ptr = new_path_copy;
  while ((token = strsep(&str_ptr, " ")) != NULL) {
    if (*token != '\0') {
      path->dirs[i] = strdup(token);
      i++;
    }
  }

  path->num_dirs = num_dirs;

  // Free the copy of the input string
  free(new_path_copy);
}

void add_directory_to_path(Path *path, const char *dir) {
  // Check if the directory is already in the path
  for (int i = 0; i < path->num_dirs; i++) {
    if (strcmp(path->dirs[i], dir) == 0) {
      // Directory is already in the path, so do nothing
      return;
    }
  }

  // Allocate space for the new directory
  char **new_dirs = malloc(sizeof(char *) * (path->num_dirs + 1));

  // Copy the old directories
  for (int i = 0; i < path->num_dirs; i++) {
    new_dirs[i] = path->dirs[i];
  }

  // Add the new directory to the end of the array
  new_dirs[path->num_dirs] = strdup(dir);

  // Free the old directory array and update the path
  free(path->dirs);
  path->dirs = new_dirs;
  path->num_dirs++;
}

void destroy_command(Command command) {
  for (size_t i = 0; i < command.num_args; i++) {
    free(command.args[i]);
  }
  free(command.args);
}

void execute_exit_command(Command command) {
  if (command.num_args == 1) {
    exit(SUCCESS);
  } else {
    print_error_message();
  }
}

void execute_cd_command(Command command) {
  if (command.num_args == 1) {
    print_error_message();
  } else {
    int result = chdir(command.args[1]);
    if (result != SUCCESS) {
      print_error_message();
    }
  }
}

void execute_path_command(Command command, Path *path) {
  if (command.num_args == 1) {
    set_path(path, "");
  } else {
    set_path(path, command.args[1]);
    for (int i = 2; i < command.num_args; i++) {
      add_directory_to_path(path, command.args[i]);
    }
  }
}

bool is_builtin(Command command) {
  return strcmp(command.args[0], "path") == 0 ||
         strcmp(command.args[0], "cd") == 0 ||
         strcmp(command.args[0], "exit") == 0;
}

void redirect_output(Command command) {
  int output_index = -1;

  // Find the index of the output redirection symbol
  for (int i = 0; i < command.num_args; i++) {
    if (strcmp(command.args[i], ">") == 0) {
      output_index = i;
      break;
    }
  }

  // Check if there is an output file specified
  if (output_index == -1 || output_index == command.num_args - 1 ||
      command.num_args - output_index != 2) {
    print_error_message();
    exit(EXIT_FAILURE);
  }

  // Duplicate the stdout stream descriptor
  int stdout_copy = dup(STDOUT_FILENO);
  if (stdout_copy == -1) {
    perror("dup");
    exit(EXIT_FAILURE);
  }

  // Open the output file for writing
  int fd =
      open(command.args[output_index + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    print_error_message();
    // perror("open");
    exit(EXIT_FAILURE);
  }

  // Redirect stdout to the output file
  if (dup2(fd, STDOUT_FILENO) == -1) {
    perror("dup2");
    exit(EXIT_FAILURE);
  }

  close(fd);

  // Restore the original stdout stream
  if (dup2(stdout_copy, STDOUT_FILENO) == -1) {
    perror("dup2");
    exit(EXIT_FAILURE);
  }

  close(stdout_copy);
}

// Executes a built-in command
void execute_builtin_command(Command command, Path *path) {
  if (strcmp(command.args[0], "exit") == 0) {
    execute_exit_command(command);
  } else if (strcmp(command.args[0], "cd") == 0) {
    execute_cd_command(command);
  } else if (strcmp(command.args[0], "path") == 0) {
    execute_path_command(command, path);
  } else {
    print_error_message();
  }
}

// Executes a non-built-in command
void execute_external_command(Command command, Path *path, int redir_flag) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  } else if (pid == 0) {
    if (path == NULL) {
      print_error_message();
      exit(EXIT_FAILURE);
    }

    char full_path[255];
    for (int i = 0; i < path->num_dirs; i++) {
      snprintf(full_path, sizeof(full_path), "%s/%s", path->dirs[i],
               command.args[0]);
      if (access(full_path, X_OK) == 0) {
        // Check if output redirection is needed
        if (redir_flag) {
          redirect_output(command);
        }
        execv(full_path, command.args);
        exit(EXIT_FAILURE);
      }
    }

    print_error_message();
    exit(EXIT_FAILURE);
  } else {
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
    }
  }
}

// Executes a command
void execute_command(Command command, Path *path) {
  int redir_flag = 0;
  for (int i = 0; i < command.num_args; i++) {
    if (strcmp(command.args[i], ">") == 0) {
      redir_flag = 1;
      break;
    }
  }
  if (is_builtin(command)) {
    execute_builtin_command(command, path);
  } else {
    // for debugging purpoes
    // print_commands(command);
    execute_external_command(command, path, redir_flag);
  }
}
