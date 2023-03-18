#ifndef UTIL_H
#define UTIL_H

#include "shell_operators.h"

typedef struct TokenList {
  char **tokens;
  int num_tokens;
  Redirection redirection;
} TokenList;

void print_tokens(TokenList tokenlist);

#endif
