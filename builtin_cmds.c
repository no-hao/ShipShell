#include "builtin_cmds.h"
#include "debug.h"
#include "errors.h"
#include "path_mgmt.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int SUCCESS = 0;

bool is_debug_cmd(const char *cmd) { return strcmp(cmd, "debug") == 0; }

bool is_exit_cmd(const char *cmd) { return strcmp(cmd, "exit") == 0; }

bool is_cd_cmd(const char *cmd) { return strcmp(cmd, "cd") == 0; }

bool is_path_cmd(const char *cmd) { return strcmp(cmd, "path") == 0; }

bool is_builtin(TokenChain *tokens) {
  const char *cmd = tokens->tokens[0];
  return is_exit_cmd(cmd) || is_cd_cmd(cmd) || is_path_cmd(cmd) ||
         is_debug_cmd(cmd);
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

void execute_debug(TokenChain *tokens) {
  if (tokens->num_tokens < 1 || tokens->num_tokens > 3) {
    print_error();
    return;
  }

  if (tokens->num_tokens == 1) {
    print_debug_status();
    return;
  }

  const char *action = tokens->tokens[1];
  if (strcmp(action, "on") == 0) {
    enable_debug();
  } else if (strcmp(action, "off") == 0) {
    disable_debug();
  } else if (strcmp(action, "toggle") == 0) {
    toggle_debug();
  } else {
    print_error();
  }
}

void execute_builtin(TokenChain *tokens) {
  const char *cmd = tokens->tokens[0];

  if (is_exit_cmd(cmd)) {
    execute_exit(tokens);
  } else if (is_cd_cmd(cmd)) {
    execute_cd(tokens);
  } else if (is_path_cmd(cmd)) {
    execute_path(tokens);
  } else if (is_debug_cmd(cmd)) {
    execute_debug(tokens);
  } else {
    print_error();
  }
}
