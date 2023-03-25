#include "path_mgmt.h"
#include "debug_and_errors.h"
#include <stdlib.h>
#include <string.h>

Path *path;

void init_path() {
  // Allocate memory for the path variable
  path = (Path *)malloc(sizeof(Path));

  // Initialize the path to /bin
  path->dirs = (char **)malloc(sizeof(char *));
  path->dirs[0] = strdup("/bin");
  path->num_dirs = 1;
}

void set_path(Path *path, const char *new_path) {
  // Free the memory used by the current path
  for (int i = 0; i < path->num_dirs; i++) {
    free(path->dirs[i]);
  }
  free(path->dirs);

  if (new_path == NULL || strcmp(new_path, "") == 0) {
    // If new path is empty, reset the path to an empty one
    path->dirs = NULL;
    path->num_dirs = 0;
  } else {
    // Split the new path into individual directories using " " as delimiter
    char *p = strdup(new_path);
    char *delim = " ";
    char *token = NULL;
    path->num_dirs = 0;
    path->dirs = NULL;
    while ((token = strsep(&p, delim)) != NULL) {
      if (strcmp(token, "") != 0) {
        path->num_dirs++;
        path->dirs =
            (char **)realloc(path->dirs, sizeof(char *) * path->num_dirs);
        path->dirs[path->num_dirs - 1] = strdup(token);
      }
    }
    free(p);
  }
}

void add_path_directory(Path *path, const char *new_path) {
  path->num_dirs++;
  path->dirs = (char **)realloc(path->dirs, sizeof(char *) * path->num_dirs);
  path->dirs[path->num_dirs - 1] = strdup(new_path);
}
