#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Command {
  char **args;
  int num_args;
} Command;

Command parse_input(const char *input, const char *delimiter);

void free_command(Command command);

#endif
