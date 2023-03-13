#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_BUF_SIZE 16

void strip_trailing_whitespace(char *s) {
  int len = strlen(s);
  while (len > 0 && isspace(s[len - 1])) {
    s[--len] = '\0';
  }
}

TokenList tokenize_input(char *input, char *delimiter) {
  char **tokens = NULL;
  char *token;
  int num_tokens = 0;
  int max_tokens = TOKEN_BUF_SIZE;

  // Allocate tokens buffer
  tokens = calloc(max_tokens, sizeof(char *));
  if (tokens == NULL) {
    perror("Error: failed to allocate memory for input tokens");
    exit(EXIT_FAILURE);
  }

  // Set default delimiter if none provided
  if (delimiter == NULL) {
    delimiter = " \t\n";
  }

  // Split input into tokens using strsep
  while ((token = strsep(&input, delimiter)) != NULL) {
    if (*token == '\0') {
      // Skip empty tokens
      continue;
    }
    if (num_tokens >= max_tokens) {
      // Expand tokens buffer if needed
      max_tokens += TOKEN_BUF_SIZE;
      tokens = realloc(tokens, sizeof(char *) * max_tokens);
      if (tokens == NULL) {
        perror("Error: failed to reallocate memory for input tokens");
        exit(EXIT_FAILURE);
      }
    }
    strip_trailing_whitespace(token);
    tokens[num_tokens++] = strdup(token);
  }

  // Create TokenList struct
  TokenList result = {tokens, num_tokens};

  // Print out the tokens
  // print_tokens(result);

  return result;
}
