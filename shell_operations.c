#include "shell_operations.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

bool process_redirection(TokenList *tokens, Redirection *redirection) {
  for (int i = 0; i < tokens->num_tokens; i++) {
    if (strcmp(tokens->tokens[i], ">") == 0) {
      // Redirect output to file
      if (i == tokens->num_tokens - 1) {
        print_error();
        return false;
      } else if (i + 2 < tokens->num_tokens) {
        print_error();
        return false;
      }
      redirection->type = OUTPUT;
      redirection->file = tokens->tokens[i + 1];
      tokens->tokens[i] = NULL;
      tokens->tokens[i + 1] = NULL;
      break;
    } else if (strcmp(tokens->tokens[i], "<") == 0) {
      // Redirect input from file
      if (i == tokens->num_tokens - 1) {
        print_error();
        return false;
      }
      redirection->type = INPUT;
      redirection->file = tokens->tokens[i + 1];
      tokens->tokens[i] = NULL;
      tokens->tokens[i + 1] = NULL;
      break;
    } else if (strstr(tokens->tokens[i], ">")) {
      // Handle cases where there are no spaces around the ">" symbol
      char *pos = strstr(tokens->tokens[i], ">");
      *pos = '\0';
      redirection->type = OUTPUT;
      redirection->file = pos + 1;
      break;
    } else if (strstr(tokens->tokens[i], "<")) {
      // Handle cases where there are no spaces around the "<" symbol
      char *pos = strstr(tokens->tokens[i], "<");
      *pos = '\0';
      redirection->type = INPUT;
      redirection->file = pos + 1;
      break;
    }
  }
  return true;
}
