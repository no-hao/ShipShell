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
  Parallel parallel = {0, NULL};

  // If the command contains only "&", return without executing anything or
  // printing an error
  if (tokens->num_tokens == 1 && strcmp(tokens->tokens[0], "&") == 0) {
    return;
  }

  // Check for parallel
  if (process_parallel(tokens, &parallel)) {
    tokens->shell_operation.type = PARALLEL;
    tokens->shell_operation.data.parallel = parallel;
  }

  // Check for redirection
  if (!process_redirection(tokens, &redirection)) {
    return;
  }

  if (tokens->shell_operation.type == PARALLEL) {
    // Execute parallel commands
    for (int i = 0; i < parallel.num_cmds; i++) {
      pid_t pid = fork();

      if (pid == -1) {
        perror("fork");
      } else if (pid == 0) {
        execute_command(&parallel.cmds[i]);
        exit(EXIT_SUCCESS);
      }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < parallel.num_cmds; i++) {
      int status;
      if (waitpid(-1, &status, 0) == -1) {
        perror("waitpid");
      }
    }

    free(parallel.cmds); // Free the memory allocated for parallel commands

  } else {
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
}
