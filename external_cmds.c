#include "external_cmds.h"
#include "errors.h"
#include "path_mgmt.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void execute_command(TokenList *tokens) {
  Redirection redirection = {REDIR_NONE, NULL};

  // Check for redirection
  if (!process_redirection(tokens, &redirection)) {
    return;
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  } else if (pid == 0) {
    if (path->num_dirs == 0) {
      print_error();
      exit(EXIT_FAILURE);
    }

    char full_path[255];
    for (int i = 0; i < path->num_dirs; i++) {
      sprintf(full_path, "%s/%s", path->dirs[i], tokens->tokens[0]);
      if (access(full_path, X_OK) == 0) {
        // Redirect input/output if necessary
        redirect(&redirection);
        if (execv(full_path, tokens->tokens) == -1) {
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
