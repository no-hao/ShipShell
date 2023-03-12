#include "parser.h"
#include "error_handler.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_TOKEN_CAPACITY 16

static int remove_trailing_whitespace(const char *s, char **result) {
  size_t len = strlen(s);
  while (len > 0 && isspace(s[len - 1])) {
    len--;
  }
  *result = malloc(len + 1);
  if (*result == NULL) {
    return -1;
  }
  memcpy(*result, s, len);
  (*result)[len] = '\0';
  return 0;
}

static char *create_token(size_t token_len, const char *start) {
  char *token = malloc(token_len + 1);
  if (token == NULL) {
    throw_parse_error(PARSE_ERROR_MEMORY);
    return NULL;
  }
  memcpy(token, start, token_len);
  token[token_len] = '\0';
  char *stripped_token;
  if (remove_trailing_whitespace(token, &stripped_token) == -1) {
    throw_parse_error(PARSE_ERROR_MEMORY);
    free(token);
    return NULL;
  }
  free(token); // free token before returning
  return stripped_token;
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
    throw_parse_error(PARSE_ERROR_MEMORY);
    for (size_t i = 0; i < *num_tokens; i++) {
      free(tokens[i]);
    }
    free(tokens); // free tokens before returning
    *num_tokens = 0;
    return NULL;
  }
  return new_tokens;
}

static const char *find_end_of_token(const char *start, const char *delimiter) {
  const char *end = start;
  int delimiter_found = 0;
  while (*end != '\0' && !delimiter_found) {
    if (strchr(delimiter, *end) != NULL) {
      delimiter_found = 1;
      if (end > start && isspace(*(end - 1))) {
        end--;
      }
    } else {
      end++;
    }
  }
  if (!delimiter_found) {
    throw_parse_error(PARSE_ERROR_TOKENS);
  }
  return end;
}

static void free_tokens(char **tokens, size_t num_tokens) {
  for (size_t i = 0; i < num_tokens; i++) {
    free(tokens[i]);
  }
  free(tokens);
}

static int process_token(const char *start, const char *end, char **tokens,
                         size_t *num_tokens) {
  size_t token_len = end - start;
  if (token_len > 0) {
    // Skip redirection operators
    if (*start == '>' || *start == '<') {
      return 1;
    }
    char *token = create_token(token_len, start);
    if (token == NULL) {
      return 0;
    }
    tokens[(*num_tokens)++] = token;
  }
  return 1;
}

static char **tokenize_input(const char *input, const char *delimiter,
                             size_t *num_tokens) {
  size_t max_tokens = 8;
  char **tokens = create_token_array(max_tokens, num_tokens);
  if (tokens == NULL) {
    return NULL;
  }

  const char *start = input;
  const char *end = NULL;
  size_t token_len = 0;
  while (*start != '\0') {
    end = find_end_of_token(start, delimiter);
    if (process_token(start, end, tokens, num_tokens) == false) {
      free_tokens(tokens, *num_tokens);
      return NULL;
    }
    start = end + 1;
  }

  tokens = resize_token_array(tokens, &max_tokens, num_tokens);
  if (tokens == NULL) {
    return NULL;
  }

  return tokens;
}

Command parse_input(const char *input, const char *delimiter) {
  size_t num_tokens = 0;
  char **tokens = tokenize_input(input, delimiter, &num_tokens);
  if (tokens == NULL) {
    throw_parse_error(PARSE_ERROR_TOKENS);
    Command command = {NULL, 0};
    return command;
  }
  Command command = {tokens, num_tokens};
  return command;
}
