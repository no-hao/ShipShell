#include "redirection.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void redirect(Redirection redirection) {
  if (redirection.in_file != NULL) {
    FILE *in_file = fopen(redirection.in_file, "r");
    if (in_file == NULL) {
      print_error();
      exit(EXIT_FAILURE);
    }
    if (dup2(fileno(in_file), STDIN_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    fclose(in_file);
  }
  if (redirection.out_file != NULL) {
    FILE *out_file = fopen(redirection.out_file, "w");
    if (out_file == NULL) {
      print_error();
      exit(EXIT_FAILURE);
    }
    if (dup2(fileno(out_file), STDOUT_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    fclose(out_file);
  }
}
