#ifndef SHELL_OPERATORS_H
#define SHELL_OPERATORS_H

typedef struct TokenList TokenList;

typedef struct Redirection {
  int in_fd;
  int out_fd;
  char *in_file;
  char *out_file;
} Redirection;

void redirect(Redirection redirection);

TokenList process_redirection(TokenList *tokens, Redirection *redirection);

#endif
