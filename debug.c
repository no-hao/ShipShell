#include "debug.h"
#include <stdio.h>

bool debug_enabled = false;

void set_debug(bool flag) { debug_enabled = flag; }

void toggle_debug() { debug_enabled = !debug_enabled; }

void debug_print(const char *message) {
  if (debug_enabled) {
    printf("[DEBUG]: %s\n", message);
  }
}

void print_token_list(const char *msg, TokenChain *tokens) {
  printf("%s:\n", msg);
  printf("format-> [index][TYPE][value]\n");
  for (int i = 0; i < tokens->num_tokens; i++) {
    printf("[%d][%s][%s] ", i,
           tokens->tokens[i] == NULL ? "OP"
           : tokens->tokens[i][0] == '&' || tokens->tokens[i][0] == '|' ||
                   tokens->tokens[i][0] == ';'
               ? "OP"
           : tokens->tokens[i][0] == '#' ? "COMMENT"
                                         : "TOKEN",
           tokens->tokens[i] == NULL ? "<null>" : tokens->tokens[i]);
  }
  printf("\n");
}
