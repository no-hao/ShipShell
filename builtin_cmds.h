#ifndef BUILTIN_CMD_H
#define BUILTIN_CMD_H

#include "util.h"
#include <stdbool.h>

bool is_builtin(TokenList *tokens);

void execute_exit(TokenList *tokens);

void execute_cd(TokenList *tokens);

void execute_path(TokenList *tokens);

void execute_builtin(TokenList *tokens);

#endif
