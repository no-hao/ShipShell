#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

typedef struct Redirection {
  int in_fd;
  int out_fd;
  char *in_file;
  char *out_file;
} Redirection;

typedef struct TokenList {
  char **tokens;
  int num_tokens;
  Redirection redirection;
} TokenList;

static void strip_trailing_whitespace(char *s);

static bool is_empty_token(const char *token);

static char *create_input_copy(const char *input);

static char **allocate_token_buffer(int max_tokens);

static char **realloc_token_buffer(char **tokens, int max_tokens);

TokenList tokenize_input(const char *input, const char *delimiter);

#endif
