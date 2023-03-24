#ifndef DEBUG_H
#define DEBUG_H

#include "shell_operations.h"
#include <stdbool.h>

extern bool debug_enabled;

void print_token_list(const char *msg, TokenChain *tokens);

void set_debug(bool flag);
void toggle_debug();
void debug_print(const char *message);

#endif /* DEBUG_H */
