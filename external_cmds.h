#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <sys/types.h>

extern const int SUCCESS;

void execute_command(TokenList *tokens);

pid_t create_child_process(void (*child_func)(TokenList *, Redirection *),
                           TokenList *tokens, Redirection *redirection);

void execute_parallel_commands(Parallel *parallel);

void execute_single_command(TokenList *tokens, Redirection *redirection);

void child_process_exec(TokenList *tokens, Redirection *redirection);

#endif
