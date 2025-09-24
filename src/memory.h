#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TYPE_I32,
  TYPE_U8,
  TYPE_F64,
} Type;

typedef union {
  int32_t as_i32;
  uint8_t as_u8;
  float_t as_float;
} Word;

typedef struct {
  Word word;
  Type type;
} Data;

typedef struct {
  void *data;
} Memory;

Memory mem_init(size_t size) { return (Memory){.data = malloc(size)}; };

void mem_set(Memory *mem, size_t addr, void *data, size_t n) {
  memmove(mem->data + addr, data, n);
}
// TODO: do for all types through X macros
int mem_get_int(Memory *mem, int addr, int n) {
  n = n + 0; // to silence the compiler
  return *(int *)(mem->data + addr);
}

void val_store_int(Memory *mem, int val, int pos) {
  mem_set(mem, sizeof(int) * pos, &val, sizeof(int));
}

int val_load_int(Memory *mem, int pos) {
  return mem_get_int(mem, sizeof(int) * pos, sizeof(int));
}

void memory_test(void) {
  Memory memory = mem_init(2048);
  val_store_int(&memory, 5, 0);
  printf("%d\n", val_load_int(&memory, 0));
  val_store_int(&memory, 6, 0);
  printf("%d\n", val_load_int(&memory, 0));
  val_store_int(&memory, 7, 1);
  printf("%d\n", val_load_int(&memory, 1));
  printf("\n");
}
