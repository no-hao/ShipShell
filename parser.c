#include "parser.h"
#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_BUF_SIZE 16

// Use 'static' for internal functions that should not be exposed to other
// source files
static void strip_trailing_whitespace(char *s) {
  int len = strlen(s);
  while (len > 0 && isspace(s[len - 1])) {
    s[--len] = '\0';
  }
}

static bool is_empty_token(const char *token) {
  // Check if the token is only composed of whitespace characters
  for (const char *c = token; *c != '\0'; ++c) {
    if (!isspace(*c)) {
      return false;
    }
  }
  return true;
}

static char *create_input_copy(const char *input) {
  char *input_copy = strdup(input);
  if (input_copy == NULL) {
    perror("Error: failed to allocate memory for input copy");
    exit(EXIT_FAILURE);
  }
  return input_copy;
}

static char **allocate_token_buffer(int max_tokens) {
  // Allocate tokens buffer
  char **tokens = calloc(max_tokens, sizeof(char *));
  if (tokens == NULL) {
    perror("Error: failed to allocate memory for input tokens");
    exit(EXIT_FAILURE);
  }
  return tokens;
}

static char **realloc_token_buffer(char **tokens, int max_tokens) {
  tokens = realloc(tokens, sizeof(char *) * max_tokens);
  if (tokens == NULL) {
    perror("Error: failed to reallocate memory for input tokens");
    exit(EXIT_FAILURE);
  }
  return tokens;
}

TokenList tokenize_input(const char *input, const char *delimiter) {
  char **tokens = NULL;
  char *token;
  int num_tokens = 0;
  int max_tokens = TOKEN_BUF_SIZE;

  // Create a modifiable copy of the input to avoid altering the original string
  char *input_copy = create_input_copy(input);
  tokens = allocate_token_buffer(max_tokens);

  // Set default delimiter if none provided
  if (delimiter == NULL) {
    delimiter = " \t\n";
  }

  char *input_ptr = input_copy;
  // Split input into tokens using strtok
  token = strtok(input_ptr, delimiter);
  while (token != NULL) {
    strip_trailing_whitespace(token);

    if (!is_empty_token(token)) {
      // Expand tokens buffer if needed
      if (num_tokens >= max_tokens) {
        max_tokens += TOKEN_BUF_SIZE;
        tokens = realloc_token_buffer(tokens, max_tokens);
      }

      char *ampersand_ptr = strchr(token, '&');
      while (ampersand_ptr != NULL) {
        if (ampersand_ptr > token) {
          tokens[num_tokens++] = strndup(token, ampersand_ptr - token);
        }
        tokens[num_tokens++] = strndup(ampersand_ptr, 1);

        token = ampersand_ptr + 1;
        ampersand_ptr = strchr(token, '&');
      }

      if (*token != '\0') {
        tokens[num_tokens++] = strdup(token);
      }
    }

    token = strtok(NULL, delimiter);
  }

  // Create TokenList struct
  TokenList result = {tokens, num_tokens};

  // Print out the tokens
  // print_token_list("tokens in tokenlist", &result);

  free(input_copy);

  return result;
}
