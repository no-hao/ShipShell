#include "command_handler.h"
#include "parser.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  char *input = NULL;
  size_t input_size = 0;
  FILE *file = stdin;
  Path *path = init_path();

  if (argc == 2 && (file = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error opening file '%s': %s\n", argv[1], strerror(errno));
    return 1;
  }

  while (file == stdin ? printf("wish> ") : 0,
         getline(&input, &input_size, file) != -1) {
    Command command = parse_input(input, " \t\n");

    execute_command(command, path);

    destroy_command(command);
  }

  if (ferror(file)) {
    fprintf(stderr, "Error reading from file: %s\n", strerror(errno));
  }

  free(input);
  fclose(file);
  free_path(path);

  return 0;
}
