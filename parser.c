#include "parser.h"
#include "error_handler.h"
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

static char **create_token_array(size_t max_tokens, size_t *num_tokens) {
  const size_t initial_size = 16;
  char **tokens = malloc(initial_size * sizeof(*tokens));
  if (tokens == NULL) {
    *num_tokens = 0;
  } else {
    *num_tokens = 0;
  }
  return tokens;
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
  free(token);
  return stripped_token;
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
    if (tokens != NULL) { // Add this check
      free(tokens);
    }
    *num_tokens = 0;
    return NULL;
  }
  return new_tokens;
}

static void determine_redirection(const char **end,
                                  RedirectionType *redir_type) {
  if (**end == '>') {
    *redir_type = REDIR_OUTPUT;
    (*end)++;
    // Skip whitespace after the redirection operator
    while (isspace(**end)) {
      (*end)++;
    }
    // Check if there is a filename after the whitespace
    if (**end == '\0') {
      throw_parse_error(PARSE_ERROR_REDIRECTION);
      exit(0);
    }
  } else if (**end == '\0') {
    throw_parse_error(PARSE_ERROR_REDIRECTION);
  }
}

static const char *find_end_of_token(const char *start, const char *delimiter) {
  const char *end = start;
  while (*end != '\0' && strchr(delimiter, *end) == NULL) {
    end++;
  }
  if (strchr(delimiter, *(end - 1)) == NULL && *end == '\0') {
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
    char *token = create_token(token_len, start);
    if (token == NULL) {
      return 0;
    }
    tokens[(*num_tokens)++] = token;
  }
  return 1;
}

static char **tokenize_input(const char *input, const char *delimiter,
                             size_t *num_tokens, RedirectionType *redir_type) {
  char **tokens = NULL;
  size_t max_tokens = 0;
  const char *start = input;
  const char *end = input;

  tokens = create_token_array(max_tokens, num_tokens);
  if (tokens == NULL) {
    throw_parse_error(PARSE_ERROR_MEMORY);
    return NULL;
  }

  Redirection redir = {REDIR_NONE, NULL};

  while (*end != '\0') {
    determine_redirection(&end, redir_type);
    if (*redir_type != REDIR_NONE) {
      end = find_end_of_token(start, delimiter);
      redir.redir_dest = create_token(end - start, start);
      if (redir.redir_dest == NULL) {
        throw_parse_error(PARSE_ERROR_MEMORY);
        free_tokens(tokens, *num_tokens);
        *num_tokens = 0;
        return NULL;
      }
      break;
    }
    end = find_end_of_token(start, delimiter);

    if (*num_tokens >= max_tokens) {
      tokens = resize_token_array(tokens, &max_tokens, num_tokens);
      if (tokens == NULL) {
        throw_parse_error(PARSE_ERROR_MEMORY);
        free_tokens(tokens, *num_tokens);
        *num_tokens = 0;
        return NULL;
      }
    }

    if (!process_token(start, end, tokens, num_tokens)) {
      throw_parse_error(PARSE_ERROR_MEMORY);
      free_tokens(tokens, *num_tokens);
      *num_tokens = 0;
      return NULL;
    }

    start = ++end;
  }

  *redir_type = redir.redir_type;
  return tokens;
}

Command parse_input(const char *input, const char *delimiter) {
  RedirectionType redir_type = REDIR_NONE;
  size_t num_tokens = 0;
  char **tokens = tokenize_input(input, delimiter, &num_tokens, &redir_type);
  if (tokens == NULL) {
    throw_parse_error(PARSE_ERROR_TOKENS);
    Command command = {NULL, 0};
    return command;
  }
  Command command = {tokens, num_tokens};
  return command;
}
