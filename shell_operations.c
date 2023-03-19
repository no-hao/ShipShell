#include "shell_operations.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**********************************/
/********** REDIRECTION **********/
/**********************************/

bool is_redirection(TokenList *tokens) {
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
  if (redirection->type == INPUT) {
    redirect_input(redirection->file);
  } else if (redirection->type == OUTPUT) {
    redirect_output(redirection->file);
  } else if (redirection->type == APPEND) {
    redirect_append(redirection->file);
  }
}

// Helper function to process output redirection
static bool process_output_redirection(TokenList *tokens,
                                       Redirection *redirection, int i) {
  if (i == tokens->num_tokens - 1) {
    print_error();
    return false;
  }
  redirection->type = OUTPUT;
  redirection->file = tokens->tokens[i + 1];
  tokens->tokens[i] = NULL;
  tokens->tokens[i + 1] = NULL;

  // Check if there are any non-NULL tokens after the output file
  for (int j = i + 2; j < tokens->num_tokens; j++) {
    if (tokens->tokens[j] != NULL) {
      print_error();
      return false;
    }
  }
  return true;
}

// Helper function to process input redirection
static bool process_input_redirection(TokenList *tokens,
                                      Redirection *redirection, int i) {
  if (i == tokens->num_tokens - 1) {
    print_error();
    return false;
  }
  redirection->type = INPUT;
  redirection->file = tokens->tokens[i + 1];
  tokens->tokens[i] = NULL;
  tokens->tokens[i + 1] = NULL;
  return true;
}

bool process_redirection(TokenList *tokens, Redirection *redirection) {
  for (int i = 0; i < tokens->num_tokens; i++) {
    if (strcmp(tokens->tokens[i], ">") == 0) {
      return process_output_redirection(tokens, redirection, i);
    } else if (strcmp(tokens->tokens[i], "<") == 0) {
      return process_input_redirection(tokens, redirection, i);
    } else if (strstr(tokens->tokens[i], ">")) {
      // Handle cases where there are no spaces around the ">" symbol
      char *pos = strstr(tokens->tokens[i], ">");
      *pos = '\0';
      redirection->type = OUTPUT;
      redirection->file = pos + 1;
      return true;
    } else if (strstr(tokens->tokens[i], "<")) {
      // Handle cases where there are no spaces around the "<" symbol
      char *pos = strstr(tokens->tokens[i], "<");
      *pos = '\0';
      redirection->type = INPUT;
      redirection->file = pos + 1;
      return true;
    }
  }
  return true;
}

/**********************************/
/************ PARALLEL ************/
/**********************************/

bool is_parallel(TokenList *tokens) {
  for (int i = 0; i < tokens->num_tokens; i++) {
    if (strcmp(tokens->tokens[i], "&") == 0) {
      tokens->tokens[i] = NULL;
      return true;
    }
  }
  return false;
}

bool process_parallel(TokenList *tokens, Parallel *parallel) {
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

int extract_command(TokenList *tokens, Parallel *parallel, int start_index,
                    int end_index, int cmd_count) {
  int cmd_length = end_index - start_index;
  if (cmd_length > 0) {
    cmd_count++;
    parallel->cmds = realloc(parallel->cmds, sizeof(TokenList) * cmd_count);
    parallel->cmds[cmd_count - 1].tokens =
        malloc((cmd_length + 1) * sizeof(char *));
    memcpy(parallel->cmds[cmd_count - 1].tokens, &tokens->tokens[start_index],
           cmd_length * sizeof(char *));
    parallel->cmds[cmd_count - 1].tokens[cmd_length] = NULL;
    parallel->cmds[cmd_count - 1].num_tokens = cmd_length;
  }
  return cmd_count;
}
