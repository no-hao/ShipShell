#include "builtin_cmds.h"
#include "errors.h"
#include "path_mgmt.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int SUCCESS = 0;

bool is_builtin(TokenChain *tokens) {
  return (strcmp(tokens->tokens[0], "path") == 0 ||
          strcmp(tokens->tokens[0], "cd") == 0 ||
          strcmp(tokens->tokens[0], "exit") == 0);
}

void execute_exit(TokenChain *tokens) {
  if (tokens->num_tokens == 1) {
    exit(SUCCESS);
  } else {
    print_error();
  }
}

void execute_cd(TokenChain *tokens) {
  if (tokens->num_tokens == 1) {
    print_error();
  } else {
    int result = chdir(tokens->tokens[1]);
    if (result != SUCCESS) {
      print_error();
    }
  }
}

void execute_path(TokenChain *tokens) {
  if (tokens->num_tokens == 1) {
    set_path(path, "");
  } else {
    set_path(path, tokens->tokens[1]);
    for (int i = 2; i < tokens->num_tokens; i++) {
      add_path_directory(path, tokens->tokens[i]);
    }
  }
}

void execute_builtin(TokenChain *tokens) {
  if (strcmp(tokens->tokens[0], "exit") == 0) {
    execute_exit(tokens);
  } else if (strcmp(tokens->tokens[0], "cd") == 0) {
    execute_cd(tokens);
  } else if (strcmp(tokens->tokens[0], "path") == 0) {
    execute_path(tokens);
  } else {
    print_error();
  }
}
