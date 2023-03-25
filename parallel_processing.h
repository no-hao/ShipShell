#ifndef PARALLEL_PROCESSING_H
#define PARALLEL_PROCESSING_H

#include "shell_types.h"

bool process_parallel(TokenChain *tokens);

bool is_parallel(TokenChain *tokens);

int extract_command(TokenChain *tokens, Parallel *parallel, int start_index,
                    int end_index, int cmd_count);

void separate_parallel_commands(TokenChain *tokens, Parallel *parallel);

void free_parallel_commands(Parallel *parallel);

#endif // ! PARALLEL_PROCESSING_H
