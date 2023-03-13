#ifndef EXECUTE_H
#define EXECUTE_H

#include "parser.h"
#include <stdbool.h>
#include <unistd.h>

extern const int ERROR;
extern const int SUCCESS;

// Define a struct to hold the path
typedef struct Path {
  char **dirs;
  int num_dirs;
} Path;

/**
 * @brief Frees the memory allocated for a Command struct.
 *
 * This function frees the memory allocated for a Command struct, including the
 * token array and any memory allocated for the redirection destination.
 *
 * @param command The Command struct to be freed.
 */
void destroy_command(Command command);

void print_commands(Command command);
Path *init_path();
void free_path(Path *path);
void set_path(Path *path, const char *new_path);
void execute_external_command(Command command, Path *path, int redir_flag);

void print_redirection_info(Command command);
bool is_builtin(Command command);
void execute_builtin(Command command, Path *path);
void execute_exit_command(Command command);
void execute_cd_command(Command command);
void execute_path_command(Command command, Path *path);

void execute_command(Command command, Path *path);

#endif
