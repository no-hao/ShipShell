#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// Initialize the local_search_path array with the default directory
char *local_search_path[] = {"/bin", NULL};

void print_error() {
  write(STDERR_FILENO, "An error has occurred\n",
        strlen("An error has occurred\n"));
}

void parse_input(char *input, char ****commands, int *command_count,
                 char ***redirection_files) {
  *commands = calloc(64, sizeof(char **));
  *redirection_files = calloc(64, sizeof(char *));
  *command_count = 0;

  char *command_string = strtok(input, "&");

  while (command_string != NULL) {
    char **args = calloc(64, sizeof(char *));
    int arg_count = 0;
    char *token = strtok(command_string, " \t\n");

    while (token != NULL) {
      if (strchr(token, '>') != NULL) {
        // Handle redirection
        if (*(token + 1) == '\0') { // Check if the next character is NULL
          token = strtok(NULL, " \t\n");
          if (token == NULL) { // If the next token is NULL, it's an error
            print_error();
            free(args);
            return;
          } else {
            (*redirection_files)[*command_count] = token;
            // Check if there's another token after the output file
            char *next_token = strtok(NULL, " \t\n");
            if (next_token != NULL && strcmp(next_token, "&") != 0) {
              // If the next token is not NULL or '&', print an error
              print_error();
              free(args);
              return;
            }
            token = next_token; // Update token to next_token
          }
        } else {
          // Additional logic for handling other redirection cases
        }
      } else {
        args[arg_count++] = token;
      }
      token = strtok(NULL, " \t\n");
    }

    args[arg_count] = NULL;
    (*commands)[*command_count] = args;
    (*command_count)++;

    command_string = strtok(NULL, "&");
  }
}

int search_executable(char *command, char *path) {
  for (int i = 0; local_search_path[i] != NULL; i++) {
    snprintf(path, 1024, "%s/%s", local_search_path[i], command);
    if (access(path, X_OK) == 0) {
      return 0;
    }
  }
  return -1;
}

int handle_builtin_command(char *command, char **args) {
  if (strcmp(command, "path") == 0) {
    int i;

    // If no arguments are passed with the path command, set local_search_path
    // to empty
    if (args[1] == NULL) {
      local_search_path[0] = NULL;
      return 1;
    }

    for (i = 0; args[i + 1] != NULL; i++) {
      // Check if the path is relative or absolute
      if (args[i + 1][0] != '/') {
        // If the path is relative, convert it to an absolute path
        char abs_path[1024];
        getcwd(abs_path, sizeof(abs_path));
        strcat(abs_path, "/");
        strcat(abs_path, args[i + 1]);
        local_search_path[i] = strdup(abs_path);
      } else {
        local_search_path[i] = strdup(args[i + 1]);
      }
    }
    local_search_path[i] = NULL;
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
    if (args[1] != NULL) {
      print_error(); // Print error if there are additional arguments for exit
      return 1;      // Return 1 to indicate the command was handled
    } else {
      exit(0);
    }
  }

  return 0;
}

int execute_command(char *command, char **args, char *redirection_file,
                    int is_parallel) {
  char path[1024];

  if (search_executable(command, path) != 0) {
    // Executable not found
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
    if (!is_parallel) {
      int status;
      waitpid(pid, &status, 0);
      if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        // print_error();
      }
    }
  } else {
    // Fork failed
    print_error();
  }

  return 0;
}

void execute_commands(char ***commands, int command_count,
                      char **redirection_files) {
  for (int i = 0; i < command_count; i++) {
    char *command = commands[i][0];
    if (handle_builtin_command(command, commands[i])) {
      continue;
    }

    int is_parallel = (i + 1 < command_count);
    if (execute_command(command, commands[i], redirection_files[i],
                        is_parallel) != 0) {
      print_error();
    }
  }
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

    char ***commands = NULL;
    int command_count = 0;
    char **redirection_files = NULL;

    parse_input(start, &commands, &command_count, &redirection_files);
    execute_commands(commands, command_count, redirection_files);

    for (int i = 0; i < command_count; i++) {
      free(commands[i]);
    }
    free(commands);
    free(redirection_files);
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
