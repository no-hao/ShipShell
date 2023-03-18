#include "shell_operations.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void redirect(Redirection redirection) {
  if (redirection.type == INPUT) {
    FILE *in_file = fopen(redirection.file, "r");
    if (in_file == NULL) {
      print_error();
      exit(EXIT_FAILURE);
    }
    if (dup2(fileno(in_file), STDIN_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    fclose(in_file);
  } else if (redirection.type == OUTPUT) {
    FILE *out_file = fopen(redirection.file, "w");
    if (out_file == NULL) {
      print_error();
      exit(EXIT_FAILURE);
    }
    if (dup2(fileno(out_file), STDOUT_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    fclose(out_file);
  } else if (redirection.type == APPEND) {
    FILE *out_file = fopen(redirection.file, "a");
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
