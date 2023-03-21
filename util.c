#include "parser.h"
#include <stdio.h>

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
