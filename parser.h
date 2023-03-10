#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  PARSE_ERROR_NONE = 0,
  PARSE_ERROR_MEMORY,
  PARSE_ERROR_TOKENS,
  PARSE_ERROR_REDIRECTION
} ParseError;

typedef struct Command {
  char **args;
  int num_args;
} Command;

/**
 * @brief Helper function to free a dynamically allocated token array.
 * @param tokens The token array to be freed.
 * @param num_tokens The number of tokens in the array.
 */
static void free_tokens(char **tokens, size_t num_tokens);

/**
 * @brief Helper function to process a single token.
 * @param start The beginning of the token.
 * @param end The end of the token.
 * @param tokens The token array to add the token to.
 * @param num_tokens The number of tokens in the token array.
 * @return True if the token was successfully processed, false otherwise.
 */
static int process_token(const char *start, const char *end, char **tokens,
                         size_t *num_tokens);

void destroy_command(Command command);

void throw_parse_error(ParseError error);

static char *create_token(size_t token_len, const char *start);

Command parse_input(const char *input, const char *delimiter);

static char **tokenize_input(const char *input, const char *delimiter,
                             size_t *num_tokens, int *redir_flag);

#endif
