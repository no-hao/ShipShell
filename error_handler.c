#include "error_handler.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void throw_parse_error(ParseError error) {
  switch (error) {
  case PARSE_ERROR_MEMORY:
    fprintf(stderr, "Error: failed to allocate memory.\n");
    break;
  case PARSE_ERROR_TOKENS:
    fprintf(stderr, "Error: failed to tokenize input.\n");
    break;
  case PARSE_ERROR_REDIRECTION:
    fprintf(stderr, "Error: invalid use of redirection.\n");
    break;
  default:
    fprintf(stderr, "Unknown parse error.\n");
  }
}

void print_error_message() {
  const char *msg = "An error has occurred\n";
  write(STDERR_FILENO, msg, strlen(msg));
}
