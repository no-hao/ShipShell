#include "parser.h"
#include "debug_and_errors.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_BUF_SIZE 16

static void strip_trailing_whitespace(char *s) {
  int len = strlen(s);
  while (len > 0 && isspace(s[len - 1])) {
    s[--len] = '\0';
  }
}

static bool is_empty_token(const char *token) {
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

static void process_special_chars(const char *token, char ***tokens,
                                  int *num_tokens) {
  char *special_char_ptr = strpbrk(token, "&><");
  while (special_char_ptr != NULL) {
    if (special_char_ptr > token) {
      (*tokens)[(*num_tokens)++] = strndup(token, special_char_ptr - token);
    }
    (*tokens)[(*num_tokens)++] = strndup(special_char_ptr, 1);

    token = special_char_ptr + 1;
    special_char_ptr = strpbrk(token, "&><");
  }

  if (*token != '\0') {
    (*tokens)[(*num_tokens)++] = strdup(token);
  }
}

static void process_token(const char *token, char ***tokens, int *num_tokens,
                          int *max_tokens) {
  if (is_empty_token(token))
    return;

  if (*num_tokens >= *max_tokens) {
    *max_tokens += TOKEN_BUF_SIZE;
    *tokens = realloc_token_buffer(*tokens, *max_tokens);
  }

  process_special_chars(token, tokens, num_tokens);
}

TokenChain tokenize_input(const char *input, const char *delimiter) {
  char **tokens = NULL;
  char *token;
  int num_tokens = 0;
  int max_tokens = TOKEN_BUF_SIZE;

  char *input_copy = create_input_copy(input);
  tokens = allocate_token_buffer(max_tokens);
  delimiter = delimiter == NULL ? " \t\n" : delimiter;

  for (token = strtok(input_copy, delimiter); token != NULL;
       token = strtok(NULL, delimiter)) {
    strip_trailing_whitespace(token);
    process_token(token, &tokens, &num_tokens, &max_tokens);
  }

  TokenChain result = {tokens, num_tokens};

  // Print out the tokens
  if (debug_enabled) {
    print_tokens("tokens in TokenChain", &result);
  }

  free(input_copy);

  return result;
}
