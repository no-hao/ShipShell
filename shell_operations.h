#ifndef SHELL_OPERATIONS_H
#define SHELL_OPERATIONS_H

#include <stdbool.h>

typedef struct TokenList TokenList; // forward declaration of TokenList struct

typedef enum { OP_NONE, PARALLEL, REDIRECTION } OperationType;

typedef enum { REDIR_NONE, INPUT, OUTPUT, APPEND } RedirectionType;

typedef struct {
  RedirectionType type;
  char *file;
} Redirection;

typedef struct {
  int num_cmds;
  TokenList *cmds;
} Parallel;

typedef struct {
  OperationType type;
  union {
    Redirection redirection;
    Parallel parallel;
  } data;
} ShellOperation;

struct TokenList {
  char **tokens;
  int num_tokens;
  ShellOperation shell_operation;
};

TokenList process_redirection(TokenList *tokens, Redirection *redirection);

TokenList process_shell_operations(TokenList tokens);

void redirect_input(const char *filename);

void redirect_output(const char *filename);

void redirect_append(const char *filename);

void redirect(Redirection *redirection);

#endif
