#ifndef PARSER_H
#define PARSER_H

typedef struct TokenList {
  char **tokens;
  int num_tokens;
} TokenList;

void strip_trailing_whitespace(char *s);

TokenList tokenize_input(char *input, char *delimiter);

#endif
