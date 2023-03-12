
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
@brief Removes trailing whitespace from a string and returns the modified
string.
@param s The input string.
@param result A pointer to the modified string. This will be dynamically
allocated and must be freed by the caller.
@return 0 on success, -1 on memory allocation error.
*/
static int remove_trailing_whitespace(const char *s, char **result);

/**
@brief Creates a token from a substring of a larger string.
@param token_len The length of the substring.
@param start A pointer to the start of the substring.
@return A pointer to the newly created token. This will be dynamically allocated
and must be freed by the caller.
*/
static char *create_token(size_t token_len, const char *start);

/**
@brief Creates an array to store tokens.
@param max_tokens The initial capacity of the array.
@param num_tokens A pointer to the number of tokens in the array.
@return A pointer to the newly created token array. This will be dynamically
allocated and must be freed by the caller.
*/
static char **create_token_array(size_t max_tokens, size_t *num_tokens);

/**
@brief Resizes an array to store tokens.
@param tokens A pointer to the existing token array.
@param max_tokens A pointer to the maximum capacity of the array.
@param num_tokens A pointer to the number of tokens in the array.
@return A pointer to the newly resized token array. This will be dynamically
allocated and must be freed by the caller.
*/
static char **resize_token_array(char **tokens, size_t *max_tokens,
                                 size_t *num_tokens);

/**
@brief Finds the end of a token within a string.
@param start A pointer to the start of the token.
@param delimiter A string containing all possible delimiters for tokens.
@return A pointer to the character following the end of the token.
*/
static const char *find_end_of_token(const char *start, const char *delimiter);

/**
@brief Frees the memory associated with an array of tokens.
@param tokens A pointer to the token array.
@param num_tokens The number of tokens in the array.
*/
static void free_tokens(char **tokens, size_t num_tokens);

/**
@brief Processes a single token and adds it to the token array.
@param start A pointer to the start of the token.
@param end A pointer to the character following the end of the token.
@param tokens A pointer to the token array.
@param num_tokens A pointer to the number of tokens in the array.
@return 1 on success, 0 on memory allocation error.
*/
static int process_token(const char *start, const char *end, char **tokens,
                         size_t *num_tokens);

/**
@brief Tokenizes the input string.
@param input The input string to tokenize.
@param delimiter A string containing all possible delimiters for tokens.
@param num_tokens A pointer to the number of tokens in the token array.
@return A pointer to the token array. This will be dynamically allocated and
must be freed by the caller.
*/
static char **tokenize_input(const char *input, const char *delimiter,
                             size_t *num_tokens);

/**
@brief Parses the input string into a command.
@param input The input string to parse.
@param delimiter A string containing all possible delimiters for tokens.
@return The parsed command as a Command struct.
*/
Command parse_input(const char *input, const char *delimiter);

#endif
