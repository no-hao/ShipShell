#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <stdbool.h>
#include <unistd.h>

extern const int ERROR;
extern const int SUCCESS;
extern const char *const ERROR_MESSAGE;
#define WRITE_ERROR_MESSAGE(msg) write(STDERR_FILENO, msg, strlen(msg))

void init_path();

bool is_builtin(TokenList tokens);

void execute_builtin(TokenList tokens);

void set_path(char **path, const char *new_path);

void execute_command(TokenList tokens);

#endif
