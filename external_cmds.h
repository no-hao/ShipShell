#ifndef EXECUTE_H
#define EXECUTE_H

#include "errors.h"
#include "parser.h"
#include "path_mgmt.h"
#include <stdbool.h>
#include <unistd.h>

extern const int ERROR;
extern const int SUCCESS;

void init_path();

void add_path_directory(Path *path, const char *new_path);

void set_path(Path *path, const char *new_path);

void redirect(Redirection redirection);

void set_path(Path *path, const char *new_path);
void execute_command(TokenList tokens);

#endif
