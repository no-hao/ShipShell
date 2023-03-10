#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

typedef enum {
  PARSE_ERROR_NONE = 0,
  PARSE_ERROR_MEMORY,
  PARSE_ERROR_TOKENS,
  PARSE_ERROR_REDIRECTION
} ParseError;

typedef struct Command {
  char **args;
  int num_args;
} Command;

void destroy_command(Command command);

Command parse_input(const char *input, const char *delimiter);

static char **tokenize_input(const char *input, const char *delimiter,
                             size_t *num_tokens, int *redir_flag);

#endif
