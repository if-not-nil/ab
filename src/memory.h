#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_TYPES(X)                                                           \
  X(int32_t, i32)                                                              \
  X(uint8_t, u8)                                                               \
  X(double, f64)

typedef enum {
#define X(c_type, name) TYPE_##name,
  MEM_TYPES(X)
#undef X
} Type;

typedef union {
  int32_t as_i32;
  uint8_t as_u8;
  float_t as_float;
} Word;

typedef struct {
  void *data;
} Memory;

Memory mem_init(size_t size) { return (Memory){.data = malloc(size)}; };

void mem_set(Memory *mem, size_t addr, const void *src, size_t n) {
  memmove((char *)mem->data + addr, src, n);
}

void mem_get(Memory *mem, size_t addr, void *dst, size_t n) {
  memcpy(dst, (char *)mem->data + addr, n);
}

#define GEN_FUNCS(c_type, enum_name)                                           \
  void val_store_##enum_name(Memory *mem, c_type val, size_t pos) {            \
    mem_set(mem, sizeof(c_type) * pos, &val, sizeof(c_type));                  \
  }                                                                            \
  c_type val_load_##enum_name(Memory *mem, size_t pos) {                       \
    c_type tmp;                                                                \
    mem_get(mem, sizeof(c_type) * pos, &tmp, sizeof(c_type));                  \
    return tmp;                                                                \
  }
MEM_TYPES(GEN_FUNCS)
#undef GEN_FUNCS

void memory_test(void) {
  Memory memory = mem_init(2048);
  val_store_i32(&memory, 5, 0);
  printf("%d\n", val_load_i32(&memory, 0));
  val_store_i32(&memory, 6, 0);
  printf("%d\n", val_load_i32(&memory, 0));
  val_store_i32(&memory, 7, 1);
  printf("%d\n", val_load_i32(&memory, 1));
  printf("\n");
}
