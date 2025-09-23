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

// levels: 0 - none, 1 - print the stack after, 2 - print every push/pop
#define LOG_LEVEL 0

#if LOG_LEVEL >= 1
#define LOG1(fmt, ...)                                                         \
  do {                                                                         \
    fprintf(stderr, "[LOG1] " fmt "\n", ##__VA_ARGS__);                        \
  } while (0)
#else
#define LOG1(fmt, ...)                                                         \
  do {                                                                         \
  } while (0)
#endif

#if LOG_LEVEL >= 2
#define LOG2(fmt, ...)                                                         \
  do {                                                                         \
    fprintf(stderr, "[LOG2] " fmt "\n", ##__VA_ARGS__);                        \
  } while (0)
#else
#define LOG2(fmt, ...)                                                         \
  do {                                                                         \
  } while (0)
#endif

#if LOG_LEVEL >= 3
#define LOG3(fmt, ...)                                                         \
  do {                                                                         \
    fprintf(stderr, "[LOG3] " fmt "\n", ##__VA_ARGS__);                        \
  } while (0)
#else
#define LOG3(fmt, ...)                                                         \
  do {                                                                         \
  } while (0)
#endif

#endif
