#ifndef PATH_MGMT_H
#define PATH_MGMT_H

typedef struct {
  char **dirs;
  int num_dirs;
} Path;

extern Path path;

void init_path();

void set_path(Path *path, const char *new_path);

void add_path_directory(Path *path, const char *new_path);

#endif // !PATH_MGMT_H
