#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <sys/types.h>

extern const int SUCCESS;

extern bool debug_enabled;

void execute_command(TokenChain *tokens);

void exec_child_process(TokenChain *tokens);

void execute_single_command(TokenChain *tokens);

void execute_parallel_commands(TokenChain *tokens);

void free_token_chain_memory(TokenChain *tokens);

pid_t create_child_process(void (*child_func)(TokenChain *),
                           TokenChain *tokens);

#endif
