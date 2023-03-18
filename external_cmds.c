#include "external_cmds.h"
#include "path_mgmt.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

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
