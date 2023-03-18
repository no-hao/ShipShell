#include "parser.h"
#include <stdio.h>

void print_token_list(const char *msg, TokenList *tokens) {
  printf("%s:\n", msg);
  for (int i = 0; i < tokens->num_tokens; i++) {
    printf("%s ", tokens->tokens[i]);
  }
  printf("\n");
}
