#include "external_cmds.h"
#include "debug_and_errors.h"
#include "parallel_processing.h"
#include "path_mgmt.h"
#include "redirection.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  char **args = malloc((tokens->num_tokens + 1) * sizeof(char *));

  int idx = 0;
  while (tokens->tokens[idx] != NULL) {
    args[idx] = tokens->tokens[idx];
    idx++;
  }
  args[idx] = NULL;

  if (tokens->shell_operation.type == REDIRECTION) {
    Redirection *redirection = &tokens->shell_operation.data.redirection;
    if (redirection->type == OUTPUT) {
      int fd = open(redirection->file, O_CREAT | O_WRONLY | O_TRUNC,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      if (fd == -1) {
        perror("Failed to open redirection file");
        exit(EXIT_FAILURE);
      }

      if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2 failed");
        exit(EXIT_FAILURE);
      }

      close(fd);
    }
  }

  // Execute the command
  if (execvp(args[0], args) == -1) {
    perror("Execution failed");
    exit(EXIT_FAILURE);
  }
  free(args);
}

void execute_command(TokenChain *tokens) {
  if (tokens->num_tokens == 1 && strcmp(tokens->tokens[0], "&") == 0) {
    return;
  }

  // First, process redirections
  if (!process_redirection(tokens)) {
    return;
  }

  // Then, check if there are parallel commands
  if (process_parallel(tokens)) {
    tokens->shell_operation.type = PARALLEL;
  }

  if (debug_enabled) {
    printf("DEBUG: Shell operation type: %d\n", tokens->shell_operation.type);
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

  for (int i = 0; i < parallel->num_cmds; i++) {
    free(parallel->cmds[i].tokens);
  }
  free(pids);
}

void execute_single_command(TokenChain *tokens) {
  char *command_path = find_command_path(tokens);
  if (command_path == NULL) {
    print_error();
    return;
  }

  tokens->tokens[0] = command_path;

  pid_t pid = create_child_process(&exec_child_process, tokens);

  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
  }

  free(command_path);
}

void free_token_chain_memory(TokenChain *tokens) {
  if (tokens->shell_operation.type == REDIRECTION) {
    Redirection *redirection = &tokens->shell_operation.data.redirection;
    free(redirection->file);
  }
}
