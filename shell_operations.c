#include "shell_operations.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**********************************/
/********** REDIRECTION **********/
/**********************************/

bool is_redirection(TokenChain *tokens) {
  for (int i = 0; i < tokens->num_tokens; i++) {
    if (strcmp(tokens->tokens[i], ">") == 0 ||
        strcmp(tokens->tokens[i], "<") == 0 ||
        strcmp(tokens->tokens[i], ">>") == 0) {
      return true;
    }
  }
  return false;
}

void redirect_input(const char *filename) {
  // printf("DEBUG: Inside redirect_input\n");
  FILE *in_file = fopen(filename, "r");
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

void redirect_append(const char *filename) {
  // printf("DEBUG: Inside redirect_append\n");
  FILE *out_file = fopen(filename, "a");
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

void redirect(Redirection *redirection) {
  // printf("DEBUG: Inside redirect\n");
  if (redirection->type == INPUT) {
    redirect_input(redirection->file);
  } else if (redirection->type == OUTPUT) {
    redirect_output(redirection->file);
  } else if (redirection->type == APPEND) {
    redirect_append(redirection->file);
  }
}

static bool process_input_redirection(TokenChain *tokens, int i) {
  if (i == tokens->num_tokens - 1) {
    print_error();
    return false;
  }
  tokens->shell_operation.type = REDIRECTION;
  tokens->shell_operation.data.redirection.type = INPUT;
  tokens->shell_operation.data.redirection.file = tokens->tokens[i + 1];
  tokens->tokens[i] = NULL;
  tokens->tokens[i + 1] = NULL;

  // Check if there are any non-NULL tokens after the input file
  for (int j = i + 2; j < tokens->num_tokens; j++) {
    if (tokens->tokens[j] != NULL) {
      print_error();
      return false;
    }
  }
  return true;
}

bool is_operator(const char *token) {
  return strcmp(token, "&") == 0 || strcmp(token, ">") == 0 ||
         strcmp(token, ">>") == 0 || strcmp(token, "<") == 0;
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
      if (i == tokens->num_tokens - 1 || tokens->tokens[i + 1] == NULL) {
        print_error();
        return false;
      }

      if (i < tokens->num_tokens - 2 && tokens->tokens[i + 2] != NULL &&
          !is_operator(tokens->tokens[i + 2])) {
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

  /* if (!tokens) { */
  /*   // printf("DEBUG: tokens is null\n"); */
  /*   return false; */
  /* } */

  if (!tokens->tokens) {
    // printf("DEBUG: tokens->tokens is null\n");
    return false;
  }

  if (index >= tokens->num_tokens - 1) {
    // printf("DEBUG: '>' is the last token, no filename provided\n");
    print_error();
    return false;
  }

  if (!tokens->tokens[index + 1] || tokens->tokens[index + 1][0] == '\0') {
    // printf("DEBUG: tokens->tokens[index + 1] is null or empty\n");
    print_error();
    return false;
  }

  if (index == tokens->num_tokens - 1) {
    print_error();
  }

  tokens->shell_operation.data.redirection.file =
      malloc(strlen(tokens->tokens[index + 1]) + 1);
  if (tokens->shell_operation.data.redirection.file == NULL) {
    // printf("DEBUG: Failed to allocate memory for redirection.file\n");
    return false;
  }

  if (!tokens->shell_operation.data.redirection.file) {
    // printf("DEBUG: tokens->shell_operation.data.redirection.file is null\n");
    return false;
  }

  strcpy(tokens->shell_operation.data.redirection.file,
         tokens->tokens[index + 1]);

  tokens->shell_operation.type = REDIRECTION;
  tokens->shell_operation.data.redirection.type = OUTPUT;

  // printf("DEBUG: Setting up output redirection to: %s\n",
  // tokens->shell_operation.data.redirection.file);
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

bool is_parallel(TokenChain *tokens) {
  bool found_ampersand = false;
  bool found_redirection = false;

  for (int i = 0; i < tokens->num_tokens; i++) {
    if (strcmp(tokens->tokens[i], "&") == 0) {
      tokens->tokens[i] = NULL;
      found_ampersand = true;
    } else if (strcmp(tokens->tokens[i], ">") == 0 ||
               strcmp(tokens->tokens[i], ">>") == 0 ||
               strcmp(tokens->tokens[i], "<") == 0) {
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
