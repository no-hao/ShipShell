#include "shell_operations.h"
#include "debug_and_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**********************************/
/********** REDIRECTION **********/
/**********************************/

void redirect(Redirection *redirection) {
  // printf("DEBUG: Inside redirect\n");
  if (redirection->type == OUTPUT) {
    redirect_output(redirection->file);
  }
}

void redirect_output(const char *filename) {
  // printf("DEBUG: Inside redirect_output\n");
  FILE *out_file = fopen(filename, "w");
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

bool is_operator(const char *token) {
  return strcmp(token, "&") == 0 || strcmp(token, ">") == 0;
}

bool is_valid_redirection(TokenChain *tokens, int i) {
  if (i < 1 || i >= tokens->num_tokens - 1) {
    return false;
  }

  // Check if the previous token is an executable
  if (access(tokens->tokens[i - 1], X_OK) != 0) {
    return false;
  }

  // Check if the next token is a valid file or path
  // You can add more conditions here, if necessary
  if (tokens->tokens[i + 1] == NULL ||
      strcmp(tokens->tokens[i + 1], "&") == -1) {
    return false;
  }

  return true;
}

bool process_redirection(TokenChain *tokens) {
  if (!tokens) {
    return false;
  }

  int i = 0;
  while (i < tokens->num_tokens) {
    if (tokens->tokens[i] == NULL) {
      i++;
      continue;
    }

    if (strcmp(tokens->tokens[i], ">") == 0) {
      if (!is_valid_redirection(tokens, i)) {
        print_error();
        return false;
      }

      if (!process_output_redirection(tokens, i)) {
        return false;
      }
    }

    i++;
  }

  return true;
}

bool process_output_redirection(TokenChain *tokens, int index) {
  // printf("DEBUG: Inside process_output_redirection\n");

  if (!tokens->tokens) {
    // printf("DEBUG: tokens->tokens is null\n");
    return false;
  }

  strcpy(tokens->shell_operation.data.redirection.file,
         tokens->tokens[index + 1]);

  tokens->shell_operation.type = REDIRECTION;
  tokens->shell_operation.data.redirection.type = OUTPUT;

  /* printf("DEBUG: Setting up output redirection to: %s\n", */
  /*        tokens->shell_operation.data.redirection.file); */
  tokens->tokens[index] = NULL;
  tokens->tokens[index + 1] = NULL;

  return true;
}

/**********************************/
/************ PARALLEL ************/
/**********************************/

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

void free_parallel_commands(Parallel *parallel) {
  for (int i = 0; i < parallel->num_cmds; i++) {
    free(parallel->cmds[i].tokens);
  }
  free(parallel->cmds);
}

// TODO: sort out this monstrosity
bool is_parallel(TokenChain *tokens) {
  bool found_ampersand = false;
  bool found_redirection = false;

  for (int i = 0; i < tokens->num_tokens; i++) {
    if (strcmp(tokens->tokens[i], "&") == 0) {
      tokens->tokens[i] = NULL;
      found_ampersand = true;
    } else if (strcmp(tokens->tokens[i], ">") == 0) {
      found_redirection = true;
    }
  }

  return found_ampersand && found_redirection;
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

void separate_parallel_commands(TokenChain *tokens, Parallel *parallel) {
  parallel->num_cmds = 0;
  parallel->cmds = NULL;

  int cmd_start = 0;
  for (int i = 0; i < tokens->num_tokens; i++) {
    if (tokens->tokens[i] == NULL) {
      parallel->num_cmds =
          extract_command(tokens, parallel, cmd_start, i, parallel->num_cmds);
      cmd_start = i + 1;
    }
  }

  if (cmd_start < tokens->num_tokens) {
    parallel->num_cmds = extract_command(
        tokens, parallel, cmd_start, tokens->num_tokens, parallel->num_cmds);
  }
}
