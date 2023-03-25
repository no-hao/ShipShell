#include "shell_types.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

void print_tokens(const char *msg, TokenChain *tokens) {
  printf("%s:\n", msg);
  printf("format-> [INDEX][TYPE][VALUE]\n");
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

void print_error() {
  write(STDERR_FILENO, "An error has occurred\n",
        strlen("An error has occurred\n"));
}
