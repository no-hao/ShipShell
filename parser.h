#ifndef PARSER_H
#define PARSER_H

#include "util.h"
#include <stdbool.h>

static void strip_trailing_whitespace(char *s);

static bool is_empty_token(const char *token);

static char *create_input_copy(const char *input);

static char **allocate_token_buffer(int max_tokens);

static void process_special_chars(const char *token, char ***tokens,
                                  int *num_tokens);

static char **realloc_token_buffer(char **tokens, int max_tokens);

static void process_token(const char *token, char ***tokens, int *num_tokens,
                          int *max_tokens);

TokenChain tokenize_input(const char *input, const char *delimiter);

#endif
