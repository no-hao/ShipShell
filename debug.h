#ifndef DEBUG_H
#define DEBUG_H

#include "shell_operations.h"

extern bool debug_enabled;

void enable_debug();

void disable_debug();

void toggle_debug();

void print_debug_status();

void debug_error();

void print_token_list(const char *msg, TokenChain *tokens);

#endif /* DEBUG_H */
