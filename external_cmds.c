#include "external_cmds.h"
#include "errors.h"
#include "path_mgmt.h"
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void execute_command(TokenList *tokens) {
  Redirection redirection = {REDIR_NONE, NULL};
  Parallel parallel = {0, NULL};

  if (tokens->num_tokens == 1 && strcmp(tokens->tokens[0], "&") == 0) {
    return;
  }

  if (process_parallel(tokens, &parallel)) {
    tokens->shell_operation.type = PARALLEL;
    tokens->shell_operation.data.parallel = parallel;
  }

  if (!process_redirection(tokens, &redirection)) {
    return;
  }

  if (tokens->shell_operation.type == PARALLEL) {
    execute_parallel_commands(&parallel);
  } else {
    execute_single_command(tokens, &redirection);
  }
}

pid_t create_child_process(void (*child_func)(TokenList *, Redirection *),
                           TokenList *tokens, Redirection *redirection) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return -1;
  } else if (pid == 0) {
    child_func(tokens, redirection);
    exit(EXIT_SUCCESS);
  }
  return pid;
}

void execute_parallel_commands(Parallel *parallel) {
  for (int i = 0; i < parallel->num_cmds; i++) {
    create_child_process(&execute_single_command, &parallel->cmds[i], NULL);
  }

  for (int i = 0; i < parallel->num_cmds; i++) {
    int status;
    if (waitpid(-1, &status, 0) == -1) {
      perror("waitpid");
    }
  }

  free(parallel->cmds);
}

void execute_single_command(TokenList *tokens, Redirection *redirection) {
  pid_t pid = create_child_process(&child_process_exec, tokens, redirection);

  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
  }
}

void child_process_exec(TokenList *tokens, Redirection *redirection) {
  if (path->num_dirs == 0) {
    print_error();
    exit(EXIT_FAILURE);
  }

  char full_path[255];
  for (int i = 0; i < path->num_dirs; i++) {
    sprintf(full_path, "%s/%s", path->dirs[i], tokens->tokens[0]);
    if (access(full_path, X_OK) == 0) {
      if (redirection) {
        redirect(redirection);
      }
      if (execv(full_path, tokens->tokens) == -1) {
        exit(EXIT_FAILURE);
      }
    }
  }
  print_error();
  exit(EXIT_FAILURE);
}
