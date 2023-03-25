#include "redirection.h"
#include "debug_and_errors.h"
#include "shell_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
  if (debug_enabled) {
    printf("DEBUG: Inside process_output_redirection\n");
  }

  if (!tokens->tokens) {
    if (debug_enabled) {
      printf("DEBUG: tokens->tokens is null\n");
    }
    return false;
  }

  strcpy(tokens->shell_operation.data.redirection.file,
         tokens->tokens[index + 1]);

  tokens->shell_operation.type = REDIRECTION;
  tokens->shell_operation.data.redirection.type = OUTPUT;

  if (debug_enabled) {
    printf("DEBUG: Setting up output redirection to: %s\n",
           tokens->shell_operation.data.redirection.file);
  }

  tokens->tokens[index] = NULL;
  tokens->tokens[index + 1] = NULL;

  return true;
}
