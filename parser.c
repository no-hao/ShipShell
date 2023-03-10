#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_TOKEN_CAPACITY 16

void destroy_command(Command command) {
  for (size_t i = 0; i < command.num_args; i++) {
    free(command.args[i]);
  }
  free(command.args);
}

static char *remove_trailing_whitespace(const char *s) {
  size_t len = strlen(s);
  while (len > 0 && isspace(s[len - 1])) {
    len--;
  }
  char *result = malloc(len + 1);
  if (result == NULL) {
    perror("Error: failed to allocate memory for result");
    return NULL;
  }
  memcpy(result, s, len);
  result[len] = '\0';
  return result;
}

static char **create_token_array(size_t max_tokens, size_t *num_tokens) {
  const size_t initial_size = 16;
  char **tokens = malloc(initial_size * sizeof(*tokens));
  if (tokens == NULL) {
    *num_tokens = 0;
  }
  return tokens;
}

static char **resize_token_array(char **tokens, size_t *max_tokens,
                                 size_t *num_tokens) {
  *max_tokens += INITIAL_TOKEN_CAPACITY;
  char **new_tokens = realloc(tokens, (*max_tokens) * sizeof(*tokens));
  if (new_tokens == NULL) {
    free(tokens);
    *num_tokens = 0;
    return NULL;
  }
  return new_tokens;
}

static char *create_token(size_t token_len, const char *start) {
  char *token = malloc(token_len + 1);
  if (token == NULL) {
    perror("Error: failed to allocate memory for token");
    return NULL;
  }
  memcpy(token, start, token_len);
  token[token_len] = '\0';
  char *stripped_token = remove_trailing_whitespace(token);
  free(token);
  return stripped_token;
}

static void determine_redirection(const char **end, int *redir_flag) {
  if (**end == '>') {
    *redir_flag = 1;
    (*end)++;
  }
}

static const char *find_end_of_token(const char *start, const char *delimiter) {
  const char *end = start;
  while (*end != '\0' && strchr(delimiter, *end) == NULL) {
    end++;
  }
  return end;
}

static char **tokenize_input(const char *input, const char *delimiter,
                             size_t *num_tokens, int *redir_flag) {
  size_t max_tokens = INITIAL_TOKEN_CAPACITY;
  char **tokens = create_token_array(max_tokens, num_tokens);
  if (tokens == NULL) {
    return NULL;
  }

  const char *start = input;
  const char *end = input;

  while (*end != '\0') {
    determine_redirection(&end, redir_flag);

    end = find_end_of_token(start, delimiter);

    if (*num_tokens >= max_tokens) {
      tokens = resize_token_array(tokens, &max_tokens, num_tokens);
      if (tokens == NULL) {
        return NULL;
      }
    }

    size_t token_len = end - start;
    if (token_len > 0) {
      char *token = create_token(token_len, start);
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
  char **tokens = tokenize_input(input, delimiter, &num_tokens, &redir_flag);
  Command command = {tokens, num_tokens};
  return command;
}
