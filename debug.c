#include "debug.h"
#include <stdio.h>

bool debug_enabled = false;

void enable_debug() {
  if (!debug_enabled) {
    debug_enabled = true;
    printf("Debugging has been enabled\n");
  } else {
    printf("Debugging is already enabled\n");
  }
}

void disable_debug() {
  if (debug_enabled) {
    debug_enabled = false;
    printf("Debugging has been disabled\n");
  } else {
    printf("Debugging is already disabled\n");
  }
}

void toggle_debug() {
  debug_enabled = !debug_enabled;
  printf("Debugging has been %s\n", debug_enabled ? "enabled" : "disabled");
}

void print_debug_status() {
  printf("Debugging is currently %s\n", debug_enabled ? "enabled" : "disabled");
}

void debug_error() { printf("Invalid command\n"); }

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
