#include <stdint.h>
#include <stdlib.h>

#define WORD_SIZE 16
#define wint uint16_t
#define MEM_SIZE 32768

typedef struct {
  void *data;
  wint word_size;
  wint stack_size;
  wint max_size;
} Mem;

void *mem_put(Mem *mem, wint ptr, void *data) { return mem + ptr; }

int main(void) {
  void *mem = malloc(MEM_SIZE);

  return 0;
}
