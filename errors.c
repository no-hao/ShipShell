#include "errors.h"
#include <string.h>
#include <unistd.h>

void print_error() {
  write(STDERR_FILENO, "An error has occurred\n",
        strlen("An error has occurred\n"));
}
