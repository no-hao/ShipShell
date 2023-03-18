#include "parser.h"
#include <stdio.h>

void print_tokens(TokenList *tokenlist) {
  for (int i = 0; i < tokenlist->num_tokens; i++) {
    printf("index: %d ", i);
    printf("token: %s\n", tokenlist->tokens[i]);
  }
}
