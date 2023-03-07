#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <stdbool.h>
#include <unistd.h>

extern const int ERROR;
extern const int SUCCESS;
extern const char *const ERROR_MESSAGE;
#define WRITE_ERROR_MESSAGE(msg) write(STDERR_FILENO, msg, strlen(msg))

// Define a struct to hold the path
typedef struct Path {
  char **dirs;
  int num_dirs;
} Path;

Path *init_path();
void free_path(Path *path);
void set_path(Path *path, const char *new_path);

bool is_builtin(Command command);
void execute_builtin(Command command, Path *path);
void exit_command(Command command);
void cd_command(Command command);
void path_command(Command command, Path *path);

void execute_command(Command command, Path *path);

#endif
