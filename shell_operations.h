#ifndef SHELL_OPERATIONS_H
#define SHELL_OPERATIONS_H

#include <stdbool.h>

typedef struct TokenChain
    TokenChain; // forward declaration of TokenChain struct

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

bool process_parallel(TokenChain *tokens);

static bool process_output_redirection(TokenChain *tokens, int i);

bool process_redirection(TokenChain *tokens);

void redirect_output(const char *filename);

void redirect(Redirection *redirection);

bool is_redirection(TokenChain *tokens);

bool is_parallel(TokenChain *tokens);

int extract_command(TokenChain *tokens, Parallel *parallel, int start_index,
                    int end_index, int cmd_count);

void separate_parallel_commands(TokenChain *tokens, Parallel *parallel);

void free_parallel_commands(Parallel *parallel);

bool is_operator(const char *token);

#endif
