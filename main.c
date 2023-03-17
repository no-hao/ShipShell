#include "builtin_cmds.h"
#include "external_cmds.h"
#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  char *input = NULL;
  size_t input_size = 0;
  int line_number = 1;
  FILE *file = stdin;
  init_path();

  if (argc > 2 || (argc == 2 && (file = fopen(argv[1], "r")) == NULL)) {
    print_error();
    exit(1);
  }

  while (file == stdin ? printf("wish> ") : 0,
         getline(&input, &input_size, file) != -1) {
    // Initialize tokens to empty list
    TokenList tokens = {.tokens = NULL, .num_tokens = 0};

    // Ignore leading whitespace
    char *start = input;
    while (*start != '\0' && isspace(*start)) {
      start++;
    }

    // Skip empty lines
    if (*start == '\0') {
      continue;
    }

    tokens = tokenize_input(start, " \t\n");
    if (is_builtin(tokens)) {
      execute_builtin(tokens);
    } else {
      execute_command(tokens);
    }

    // Free tokens
    free(tokens.tokens);
  }

  // Check for file errors and close file
  if (ferror(file)) {
    print_error();
  }

  fclose(file);

  // Free input
  free(input);

  return 0;
}
