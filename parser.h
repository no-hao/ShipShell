#ifndef PARSER_H
#define PARSER_H

typedef struct Redirection {
  int in_fd;
  int out_fd;
  char *in_file;
  char *out_file;
} Redirection;

typedef struct TokenList {
  char **tokens;
  int num_tokens;
  Redirection redirection;
} TokenList;

void strip_trailing_whitespace(char *s);

TokenList tokenize_input(char *input, char *delimiter);

#endif
