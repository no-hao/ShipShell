#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "shell_types.h"

static bool process_output_redirection(TokenChain *tokens, int i);

bool process_redirection(TokenChain *tokens);

void redirect_output(const char *filename);

void redirect(Redirection *redirection);

bool is_redirection(TokenChain *tokens);

bool is_operator(const char *token);

#endif
