#include "command_handler.h"
#include "parser.h"
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

  if (argc == 2 && (file = fopen(argv[1], "r")) == NULL) {
    print_error( );
    return 1;
  }

  while (file == stdin ? printf("wish> ") : 0,
         getline(&input, &input_size, file) != -1) {
    TokenList tokens = tokenize_input(input, " \t\n");

    if (is_builtin(tokens)) {
      execute_builtin(tokens);
    } else {
      execute_command(tokens);
    }

    free(tokens.tokens);
  }

  if (ferror(file)) {
    print_error( );
  }

  free(input);
  fclose(file);

  return 0;
}
