#ifndef BUILTIN_CMD_H
#define BUILTIN_CMD_H

#include "util.h"
#include <stdbool.h>

bool is_builtin(TokenChain *tokens);

void execute_exit(TokenChain *tokens);

void execute_cd(TokenChain *tokens);

void execute_path(TokenChain *tokens);

void execute_builtin(TokenChain *tokens);

#endif
