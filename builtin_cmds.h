#ifndef BUILTIN_CMD_H
#define BUILTIN_CMD_H

#include "debug_and_errors.h"
#include <stdbool.h>

bool is_exit_cmd(const char *cmd);

bool is_cd_cmd(const char *cmd);

bool is_path_cmd(const char *cmd);

bool is_builtin(TokenChain *tokens);

void execute_exit(TokenChain *tokens);

void execute_cd(TokenChain *tokens);

void execute_path(TokenChain *tokens);

void execute_builtin(TokenChain *tokens);

#endif
