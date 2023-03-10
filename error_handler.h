#ifndef ERRORS_H
#define ERRORS_H

#include <stddef.h>
#include <string.h>
#include <unistd.h>

typedef enum {
  PARSE_ERROR_NONE = 0,
  PARSE_ERROR_MEMORY,
  PARSE_ERROR_TOKENS,
  PARSE_ERROR_REDIRECTION
} ParseError;

void print_error_message();

/**
 * @brief Throws a parse error and prints an error message to stderr.
 *
 * This function throws a parse error and prints an error message to
 * stderr based on the type of error passed in.
 *
 * @param error The type of parse error.
 */
void throw_parse_error(ParseError error);

#endif
