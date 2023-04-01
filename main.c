#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *local_search_path = "/bin";

// Utility functions
void print_error() {
  write(STDERR_FILENO, "An error has occurred\n",
        strlen("An error has occurred\n"));
}

// Utility functions
void parse_input(char *input, char **command, char ***args,
                 char **redirection_file, int *is_parallel) {
  *is_parallel = 0;
  *redirection_file = NULL;

  // Tokenize the input string
  char *token = strtok(input, " \t\n");

  // Get the command
  *command = token;

  // Get the arguments
  int arg_count = 0;

  // Add the command to the args array
  (*args)[arg_count++] = *command;

  while ((token = strtok(NULL, " \t\n")) != NULL) {
    if (strcmp(token, ">") == 0) {
      // Handle redirection
      token = strtok(NULL, " \t\n");
      if (token != NULL) {
        *redirection_file = token;
      }
      break;
    } else if (strcmp(token, "&") == 0) {
      // Handle parallel commands
      *is_parallel = 1;
      break;
    } else {
      // Add the argument to the args array
      (*args)[arg_count++] = token;
    }
  }

  // Null-terminate the args array
  (*args)[arg_count] = NULL;
}

int search_executable(char *command, char *path) {
  snprintf(path, 1024, "%s/%s", local_search_path, command);
  if (access(path, X_OK) == 0) {
    return 0;
  }
  return -1;
}

int execute_command(char *command, char **args, char *redirection_file) {
  char path[1024];

  if (search_executable(command, path) != 0) {
    return -1;
  }

  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    if (redirection_file) {
      int fd = open(redirection_file, O_CREAT | O_WRONLY | O_TRUNC,
                    S_IRUSR | S_IWUSR);
      if (fd == -1) {
        print_error();
        exit(1);
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
    execv(path, args);
    // If execv returns, it means there was an error
    // Remove print_error(); to let the executable handle the error message
    exit(1);
  } else if (pid > 0) {
    // Parent process
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
      // print_error();
    }
  } else {
    // Fork failed
    print_error();
  }

  return 0;
}

int handle_builtin_command(char *command, char **args) {
  if (strcmp(command, "path") == 0) {
    if (args[1] == NULL) {
      // If no argument is provided, clear the search path
      local_search_path[0] = '\0';
    } else {
      // Set the local search path to the first provided argument
      strncpy(local_search_path, args[1], 1024);
      local_search_path[1023] = '\0';
    }
    return 1;
  } else if (strcmp(command, "cd") == 0) {
    if (args[1] == NULL) {
      print_error(); // Print error if no arguments are provided for cd
    } else if (args[2] != NULL) {
      print_error(); // Print error if there are more than 1 arguments for cd
    } else {
      if (chdir(args[1]) != 0) {
        perror("wish");
      }
    }
    return 1;
  }

  if (strcmp(command, "exit") == 0) {
    exit(0);
  }

  return 0;
}

void handle_redirection(/* arguments */) {
  // Implement redirection
}

void handle_parallel_commands(/* arguments */) {
  // Implement parallel commands
}

int main(int argc, char *argv[]) {
  char *input = NULL;
  size_t input_size = 0;
  int line_number = 1;
  FILE *file = stdin;

  if (argc > 2 || (argc == 2 && (file = fopen(argv[1], "r")) == NULL)) {
    print_error();
    exit(1);
  }

  while (file == stdin ? printf("wish> ") : 0,
         getline(&input, &input_size, file) != -1) {

    // Ignore leading whitespace
    char *start = input;
    while (*start != '\0' && isspace(*start)) {
      start++;
    }

    // Skip empty lines
    if (*start == '\0') {
      continue;
    }

    char *command = NULL;
    char **args = calloc(64, sizeof(char *));
    char *redirection_file = NULL;
    int is_parallel = 0;

    parse_input(start, &command, &args, &redirection_file, &is_parallel);

    if (!handle_builtin_command(command, args)) {
      if (execute_command(command, args, redirection_file) != 0) {
        print_error();
      }
    }

    free(args);
  }

  // Check for file errors and close file
  if (ferror(file)) {
    print_error();
  }

  if (file != stdin) {
    fclose(file);
  }

  // Free input
  free(input);

  return 0;
}
