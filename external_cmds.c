#include "external_cmds.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

const int ERROR = 1;
const int SUCCESS = 0;

Path path;

void init_path() {
  path.dirs = (char **)malloc(sizeof(char *));
  path.dirs[0] = strdup("/bin");
  path.num_dirs = 1;
}

bool is_builtin(TokenList tokens) {
  return (strcmp(tokens.tokens[0], "path") == 0 ||
          strcmp(tokens.tokens[0], "cd") == 0 ||
          strcmp(tokens.tokens[0], "exit") == 0);
}

void set_path(Path *path, const char *new_path) {
  // Free the memory used by the current path
  for (int i = 0; i < path->num_dirs; i++) {
    free(path->dirs[i]);
  }
  free(path->dirs);

  if (new_path == NULL || strcmp(new_path, "") == 0) {
    // If new path is empty, reset the path to an empty one
    path->dirs = NULL;
    path->num_dirs = 0;
  } else {
    // Split the new path into individual directories using " " as delimiter
    char *p = strdup(new_path);
    char *delim = " ";
    char *token = NULL;
    path->num_dirs = 0;
    path->dirs = NULL;
    while ((token = strsep(&p, delim)) != NULL) {
      if (strcmp(token, "") != 0) {
        path->num_dirs++;
        path->dirs =
            (char **)realloc(path->dirs, sizeof(char *) * path->num_dirs);
        path->dirs[path->num_dirs - 1] = strdup(token);
      }
    }
    free(p);
  }
}

void add_path_directory(Path *path, const char *new_path) {
  path->num_dirs++;
  path->dirs = (char **)realloc(path->dirs, sizeof(char *) * path->num_dirs);
  path->dirs[path->num_dirs - 1] = strdup(new_path);
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
      set_path(&path, "");
    } else {
      set_path(&path, tokens.tokens[1]);
      for (int i = 2; i < tokens.num_tokens; i++) {
        add_path_directory(&path, tokens.tokens[i]);
      }
    }
  } else {
    print_error();
  }
}

void redirect(Redirection redirection) {
  if (redirection.in_file != NULL) {
    FILE *in_file = fopen(redirection.in_file, "r");
    if (in_file == NULL) {
      print_error();
      exit(EXIT_FAILURE);
    }
    if (dup2(fileno(in_file), STDIN_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    fclose(in_file);
  }
  if (redirection.out_file != NULL) {
    FILE *out_file = fopen(redirection.out_file, "w");
    if (out_file == NULL) {
      print_error();
      exit(EXIT_FAILURE);
    }
    if (dup2(fileno(out_file), STDOUT_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    fclose(out_file);
  }
}

void execute_command(TokenList tokens) {
  Redirection redirection = {0};

  // Check for redirection
  for (int i = 0; i < tokens.num_tokens; i++) {
    if (strcmp(tokens.tokens[i], ">") == 0) {
      // Redirect output to file
      if (i == tokens.num_tokens - 1) {
        print_error();
        return;
      } else if (i + 2 < tokens.num_tokens) {
        print_error();
        return;
      }
      redirection.out_file = tokens.tokens[i + 1];
      tokens.tokens[i] = NULL;
      tokens.tokens[i + 1] = NULL;
      break;
    } else if (strcmp(tokens.tokens[i], "<") == 0) {
      // Redirect input from file
      if (i == tokens.num_tokens - 1) {
        print_error();
        return;
      }
      redirection.in_file = tokens.tokens[i + 1];
      tokens.tokens[i] = NULL;
      tokens.tokens[i + 1] = NULL;
      break;
    } else if (strstr(tokens.tokens[i], ">")) {
      // Handle cases where there are no spaces around the ">" symbol
      char *pos = strstr(tokens.tokens[i], ">");
      *pos = '\0';
      redirection.out_file = pos + 1;
      break;
    } else if (strstr(tokens.tokens[i], "<")) {
      // Handle cases where there are no spaces around the "<" symbol
      char *pos = strstr(tokens.tokens[i], "<");
      *pos = '\0';
      redirection.in_file = pos + 1;
      break;
    }
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  } else if (pid == 0) {
    if (path.num_dirs == 0) {
      print_error();
      exit(EXIT_FAILURE);
    }

    char full_path[255];
    for (int i = 0; i < path.num_dirs; i++) {
      sprintf(full_path, "%s/%s", path.dirs[i], tokens.tokens[0]);
      if (access(full_path, X_OK) == 0) {
        // Redirect input/output if necessary
        redirect(redirection);
        if (execv(full_path, tokens.tokens) == -1) {
          // print_error();
          exit(EXIT_FAILURE);
        }
      }
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
