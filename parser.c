#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_BUF_SIZE 16

void free_command(Command command) {
  for (size_t i = 0; i < command.num_args; i++) {
    free(command.args[i]);
  }
  free(command.args);
}

static void strip_trailing_whitespace(char *s) {
  size_t len = strlen(s);
  while (len > 0 && isspace(s[len - 1])) {
    s[--len] = '\0';
  }
}

static char **initialize_token_pointers(size_t max_tokens, size_t *num_tokens) {
  char **tokens = malloc(max_tokens * sizeof(*tokens));
  if (tokens == NULL) {
    perror("Error: failed to allocate memory for tokens");
    *num_tokens = 0;
  }
  return tokens;
}

static char **realloc_tokens(char **tokens, size_t *max_tokens,
                             size_t *num_tokens) {
  *max_tokens += TOKEN_BUF_SIZE;
  char **new_tokens = realloc(tokens, (*max_tokens) * sizeof(*tokens));
  if (new_tokens == NULL) {
    perror("Error: failed to reallocate memory for tokens");
    free(tokens);
    *num_tokens = 0;
    return NULL;
  }
  return new_tokens;
}

static char *allocate_token(size_t token_len, const char *start) {
  char *token = malloc(token_len + 1);
  if (token == NULL) {
    perror("Error: failed to allocate memory for token");
    return NULL;
  }
  memcpy(token, start, token_len);
  token[token_len] = '\0';
  strip_trailing_whitespace(token);
  return token;
}

static char **split_string(const char *input, const char *delimiter,
                           size_t *num_tokens, int *redir_flag) {
  size_t max_tokens = TOKEN_BUF_SIZE;
  char **tokens = initialize_token_pointers(max_tokens, num_tokens);
  if (tokens == NULL) {
    return NULL;
  }

  const char *start = input;
  const char *end = input;

  while (*end != '\0') {
    if (*end == '>') {
      *redir_flag = 1;
      end++;
      continue;
    }

    while (*end != '\0' && strchr(delimiter, *end) == NULL) {
      end++;
    }

    if (*num_tokens >= max_tokens) {
      tokens = realloc_tokens(tokens, &max_tokens, num_tokens);
      if (tokens == NULL) {
        return NULL;
      }
    }

    size_t token_len = end - start;
    if (token_len > 0) {
      char *token = allocate_token(token_len, start);
      if (token == NULL) {
        for (size_t i = 0; i < *num_tokens; i++) {
          free(tokens[i]);
        }
        free(tokens);
        *num_tokens = 0;
        return NULL;
      }
      tokens[(*num_tokens)++] = token;
    }

    start = ++end;
  }

  return tokens;
}

Command parse_input(const char *input, const char *delimiter) {
  int redir_flag = 0;
  size_t num_tokens = 0;
  char **tokens = split_string(input, delimiter, &num_tokens, &redir_flag);
  Command command = {tokens, num_tokens};
  return command;
}
