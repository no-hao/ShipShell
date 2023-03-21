#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <sys/types.h>

extern const int SUCCESS;

void execute_command(TokenList *tokens);

void exec_child_process(TokenList *tokens);

void execute_single_command(TokenList *tokens);

void execute_parallel_commands(TokenList *tokens);

pid_t create_child_process(void (*child_func)(TokenList *), TokenList *tokens);

#endif
