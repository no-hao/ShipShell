#include "parallel_processing.h"
#include "debug_and_errors.h"
#include "shell_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void free_parallel_commands(Parallel *parallel) {
  for (int i = 0; i < parallel->num_cmds; i++) {
    free(parallel->cmds[i].tokens);
  }
  free(parallel->cmds);
}

bool process_parallel(TokenChain *tokens) {
  Parallel *parallel = &tokens->shell_operation.data.parallel;
  int cmd_count = 0;
  int start_index = 0;

  // Initialize cmds to NULL
  parallel->cmds = NULL;

  for (int i = 0; i < tokens->num_tokens; i++) {
    if (tokens->tokens[i] == NULL) {
      continue;
    }

    if (strcmp(tokens->tokens[i], "&") == 0) {
      cmd_count = extract_command(tokens, parallel, start_index, i, cmd_count);
      start_index = i + 1;
    }
  }

  if (start_index != tokens->num_tokens &&
      tokens->num_tokens - start_index > 0) {
    cmd_count = extract_command(tokens, parallel, start_index,
                                tokens->num_tokens, cmd_count);
  }

  parallel->num_cmds = cmd_count;

  return cmd_count > 1;
}

int extract_command(TokenChain *tokens, Parallel *parallel, int start_index,
                    int end_index, int cmd_count) {
  int cmd_length = end_index - start_index;
  if (cmd_length > 0) {
    cmd_count++;
    parallel->cmds = realloc(parallel->cmds, sizeof(TokenChain) * cmd_count);
    parallel->cmds[cmd_count - 1].tokens =
        malloc((cmd_length + 1) * sizeof(char *));
    memcpy(parallel->cmds[cmd_count - 1].tokens, &tokens->tokens[start_index],
           cmd_length * sizeof(char *));
    parallel->cmds[cmd_count - 1].tokens[cmd_length] = NULL;
    parallel->cmds[cmd_count - 1].num_tokens = cmd_length;
  }
  return cmd_count;
}
