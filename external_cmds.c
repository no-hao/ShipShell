#include "external_cmds.h"
#include "parallel_processing.h"
#include "path_mgmt.h"
#include "redirection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *find_command_path(TokenChain *tokens) {
  char full_path[255];
  for (int i = 0; i < path->num_dirs; i++) {
    sprintf(full_path, "%s/%s", path->dirs[i], tokens->tokens[0]);
    if (access(full_path, X_OK) == 0) {
      return strdup(full_path);
    }
  }
  return NULL;
}

pid_t create_child_process(void (*child_func)(TokenChain *),
                           TokenChain *tokens) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return -1;
  } else if (pid == 0) {
    child_func(tokens);
    exit(EXIT_SUCCESS);
  }
  return pid;
}

void exec_child_process(TokenChain *tokens) {
  if (path->num_dirs == 0) {
    print_error();
    exit(EXIT_FAILURE);
  }

  char *full_path = find_command_path(tokens);
  if (full_path == NULL) {
    print_error();
    exit(EXIT_FAILURE);
  }

  if (tokens->shell_operation.type == REDIRECTION) {
    redirect(&tokens->shell_operation.data.redirection);
  }
  if (execv(full_path, tokens->tokens) == -1) {
    exit(EXIT_FAILURE);
  }
}

void execute_command(TokenChain *tokens) {
  if (tokens->num_tokens == 1 && strcmp(tokens->tokens[0], "&") == 0) {
    return;
  }

  if (process_parallel(tokens)) {
    tokens->shell_operation.type = PARALLEL;
  }
  if (!process_redirection(tokens)) {
    return;
  }

  if (tokens->shell_operation.type == PARALLEL) {
    execute_parallel_commands(tokens);
  } else {
    execute_single_command(tokens);
  }
}

void execute_parallel_commands(TokenChain *tokens) {
  Parallel *parallel = &tokens->shell_operation.data.parallel;
  pid_t *pids = malloc(sizeof(pid_t) * parallel->num_cmds);

  for (int i = 0; i < parallel->num_cmds; i++) {
    pids[i] = create_child_process(&execute_single_command, &parallel->cmds[i]);
  }

  // Wait for all child processes to complete
  for (int i = 0; i < parallel->num_cmds; i++) {
    int status;
    if (waitpid(pids[i], &status, 0) == -1) {
      perror("waitpid");
    }
  }

  free(parallel->cmds);
  free(pids);
}

void execute_single_command(TokenChain *tokens) {
  pid_t pid = create_child_process(&exec_child_process, tokens);

  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
  }
}
