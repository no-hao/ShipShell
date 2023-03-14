#ifndef EXECUTE_H
#define EXECUTE_H

#include "errors.h"
#include "parser.h"
#include <stdbool.h>
#include <unistd.h>

typedef struct {
  char **dirs;
  int num_dirs;
} Path;

extern const int ERROR;
extern const int SUCCESS;

void init_path();

bool is_builtin(TokenList tokens);
void add_path_directory(Path *path, const char *new_path);

void execute_builtin(TokenList tokens);

void set_path(Path *path, const char *new_path);

void set_path(Path *path, const char *new_path);
void execute_command(TokenList tokens);

#endif
