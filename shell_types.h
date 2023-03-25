#ifndef SHELL_TYPES_H
#define SHELL_TYPES_H

#include <stdbool.h>

// forward declaration of TokenChain struct
typedef struct TokenChain TokenChain;

typedef enum { OP_NONE, PARALLEL, REDIRECTION } OperationType;

typedef enum { REDIR_NONE, INPUT, OUTPUT, APPEND } RedirectionType;

typedef struct {
  RedirectionType type;
  char *file;
} Redirection;

typedef struct {
  int num_cmds;
  TokenChain *cmds;
} Parallel;

typedef struct {
  OperationType type;
  union {
    Redirection redirection;
    Parallel parallel;
  } data;
} ShellOperation;

struct TokenChain {
  char **tokens;
  int num_tokens;
  ShellOperation shell_operation;
};

#endif
