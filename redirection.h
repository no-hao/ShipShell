#ifndef REDIRECTION_H
#define REDIRECTION_H

typedef struct Redirection {
  int in_fd;
  int out_fd;
  char *in_file;
  char *out_file;
} Redirection;

#endif // !REDIRECTION_H
