#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
// note that with static inline it will only be included once
static inline void die(const char *msg) {
  fprintf(stderr, "\033[31m%s\033[0m\n", msg);
  exit(1);
}
#define chkdie(x, err)                                                         \
  if (x)                                                                       \
  die(err)
#endif
