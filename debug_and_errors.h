#ifndef DEBUG_H
#define DEBUG_H

#include "shell_operations.h"

extern bool debug_enabled;

void print_error();

void debug_error();

void enable_debug();

void toggle_debug();

void disable_debug();

void print_debug_status();

void print_tokens(const char *msg, TokenChain *tokens);

#endif /* DEBUG_H */
