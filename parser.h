#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

typedef struct Command {
  char **args;
  int num_args;
} Command;

void free_command(Command command);

Command parse_input(const char *input, const char *delimiter);

static char **split_string(const char *input, const char *delimiter,
                           size_t *num_tokens, int *redir_flag);

#endif
