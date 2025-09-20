#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  INST_PUSH,  // push a number onto the stack
  INST_POP,   // pop a number off the top
  INST_DUP,   // pop the last number then push it twice
  INST_ADD,   // pop two numbers then push a + b
  INST_SUB,   // pop two numbers then push a - b
  INST_MUL,   // pop two numbers then push a * b
  INST_DIV,   // pop two numbers then push a / b
  INST_SWAP,  // pop a then b, push b then a
  INST_PRINT, // pop the last number and print it
  INST_HALT,  // special halt (NYI)
} INST_SET;

typedef struct {
  INST_SET type;
  int val;
} INST;

#define MAX_STACK_SIZE 1024

typedef struct {
  int stack[MAX_STACK_SIZE];
  int stack_size;
  size_t program_size;
  INST *instructions;
} Machine;

#define DEF_INST_PUSH(x) {.type = INST_PUSH, .val = x}
#define DEF_INST_PRINT {.type = INST_PUSH}
#define DEF_INST_SWAP {.type = INST_SWAP}
#define DEF_INST_POP {.type = INST_POP}
#define DEF_INST_ADD {.type = INST_ADD}
#define DEF_INST_SUB {.type = INST_SUB}
#define DEF_INST_MUL {.type = INST_MUL}
#define DEF_INST_DUP {.type = INST_DUP}
#define DEF_INST_DIV {.type = INST_DIV}

INST program[] = {
    DEF_INST_PUSH(5),
    DEF_INST_PUSH(10),
    DEF_INST_SWAP,
    DEF_INST_SUB,
};

#define PROGRAM_SIZE (sizeof(program) / sizeof(program[0]))

void rt_err(const char *msg) {
  fprintf(stderr, "RUNTIME ERROR: %s", msg);
  exit(1);
}

void print_stack(Machine *machine) {
  printf("\n=== stack size %d:\n", machine->stack_size);
  for (int i = 0; i <= machine->stack_size - 1; i++) {
    printf("%d ", machine->stack[i]);
  }
  putc('\n', stdout);
}

void prog_write_to_file(Machine *machine, char *path) {
  FILE *file = fopen(path, "wb");
  if (file == NULL) {
    fprintf(stderr, "\ncoudlnt write file!!\n");
    exit(1);
  }
  fwrite(machine->instructions, sizeof(machine->instructions[0]), PROGRAM_SIZE,
         file);
  fclose(file);
}

void prog_read_from_file(Machine *machine, char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "\ncoudlnt read file!!\n");
    exit(1);
  }
  INST *instructions = malloc(sizeof(INST) * MAX_STACK_SIZE);

  fseek(file, 0, SEEK_END);
  int len = ftell(file);
  fseek(file, 0, SEEK_SET);
  fread(instructions, sizeof(instructions[0]), len / 8, file);

  machine->program_size = len / 8;
  machine->instructions = instructions;

  fclose(file);
}

// operations
void push(Machine *machine, int value) {
  printf("LOG: pushing %d to %d\n", value, machine->stack_size);
  if (machine->stack_size >= MAX_STACK_SIZE)
    rt_err("stack overflow!");

  machine->stack[machine->stack_size] = value;
  machine->stack_size++;
}

int pop(Machine *machine) {
  printf("LOG: popping\n");
  if (machine->stack_size <= 0)
    rt_err("stack underflow!");

  machine->stack_size--;
  return machine->stack[machine->stack_size];
}

int main(void) {
  int a, b;
  Machine *m = malloc(sizeof(Machine));
  // prog_read_from_file(m, "./prog.ab");
  m->instructions = program;
  m->program_size = PROGRAM_SIZE;
  prog_write_to_file(m, "./prog.ab");
  for (size_t ip = 0; ip < m->program_size; ip++) {
    // ip for instruction pointer
    switch (m->instructions[ip].type) {
    case INST_PUSH:
      push(m, m->instructions[ip].val);
      break;
    case INST_POP:
      pop(m);
      break;
    case INST_ADD:
      a = pop(m);
      b = pop(m);
      push(m, a + b);
      break;
    case INST_DUP:
      a = pop(m); // cuz u shouldnt peek into the stack
      push(m, a);
      push(m, a);
      break;
    case INST_SUB:
      a = pop(m);
      b = pop(m);
      push(m, b - a);
      break;
    case INST_MUL:
      a = pop(m);
      b = pop(m);
      push(m, a * b);
      break;
    case INST_DIV:
      a = pop(m);
      b = pop(m);
      push(m, b / a);
      break;
    case INST_SWAP:
      a = pop(m);
      b = pop(m);
      push(m, a);
      push(m, b);
      break;
    case INST_PRINT:
      printf("OUT: %d\n", pop(m)); // TODO: abstract
      break;
    case INST_HALT:
      exit(0);
      break;
    }
  };
  print_stack(m);
  return 0;
}
