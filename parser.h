#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Command {
  char **args;
  int num_args;
} Command;

static char **split_string(const char *input, const char *delimiter,
                           size_t *num_tokens, int *redir_flag);
Command parse_input(const char *input, const char *delimiter);

void free_command(Command command);

#endif
