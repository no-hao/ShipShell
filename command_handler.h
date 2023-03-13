#ifndef EXECUTE_H
#define EXECUTE_H

#include "errors.h"
#include "parser.h"
#include <stdbool.h>
#include <unistd.h>

extern const int ERROR;
extern const int SUCCESS;

void init_path();

bool is_builtin(TokenList tokens);

void execute_builtin(TokenList tokens);

void set_path(char **path, const char *new_path);

void execute_command(TokenList tokens);

#endif
