#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  REDIR_NONE = 0,
  REDIR_OUTPUT,
  REDIR_INPUT,
  REDIR_APPEND
} RedirectionType;

typedef struct {
  RedirectionType redir_type;
  char *redir_dest;
} Redirection;

typedef struct {
  char **args;
  size_t num_args;
  Redirection redirection;
} Command;

/**
 * @brief Helper function to free a dynamically allocated token array.
 * @param tokens The token array to be freed.
 * @param num_tokens The number of tokens in the array.
 */
static void free_tokens(char **tokens, size_t num_tokens);

/**
 * @brief Helper function to process a single token.
 *
 * This function creates a token from a given start and end position and adds it
 * to the token array.
 *
 * @param start The beginning of the token.
 * @param end The end of the token.
 * @param tokens The token array to add the token to.
 * @param num_tokens The number of tokens in the token array.
 * @return True if the token was successfully processed, false otherwise.
 */
static int process_token(const char *start, const char *end, char **tokens,
                         size_t *num_tokens);

/**
 * @brief Frees the memory allocated for a Command struct.
 *
 * This function frees the memory allocated for a Command struct, including the
 * token array and any memory allocated for the redirection destination.
 *
 * @param command The Command struct to be freed.
 */
void destroy_command(Command command);

/**
 * @brief Creates a token from a given start position and length.
 *
 * This function creates a token from a given start position and length.
 * It removes any trailing whitespace from the token and returns a pointer to
 * the new token.
 *
 * @param token_len The length of the token.
 * @param start The beginning of the token.
 * @return A pointer to the new token, or NULL if memory allocation failed.
 */
static char *create_token(size_t token_len, const char *start);

/**
 * @brief Parses input and creates a command struct.
 *
 * This function tokenizes input using a given delimiter and returns a Command
 * struct containing an array of tokens and the number of tokens. If redirection
 * is detected, the redirection field of the Command struct will be set
 * accordingly.
 *
 * @param input The input string to be parsed.
 * @param delimiter The delimiter to use for tokenizing the input string.
 * @return A Command struct containing the token array, number of tokens, and
 * any detected redirection.
 */
Command parse_input(const char *input, const char *delimiter);

/**
 * @brief Tokenizes input using a given delimiter.
 *
 * This function tokenizes input using a given delimiter and returns an array of
 * tokens and the number of tokens. If redirection is detected, the redir_type
 * pointer will be set accordingly.
 *
 * @param input The input string to be tokenized.
 * @param delimiter The delimiter to use for tokenizing the input string.
 * @param num_tokens A pointer to the variable to store the number of tokens.
 * @param redir_type A pointer to the variable to store the type of redirection,
 * if any.
 * @return An array of tokens, or NULL if memory allocation failed.
 */
static char **tokenize_input(const char *input, const char *delimiter,
                             size_t *num_tokens, RedirectionType *redir_type);

#endif
