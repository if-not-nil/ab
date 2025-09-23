// yo remember to move things to separate header files when the length reaches
// 500 lines
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "instructions.h"
#include "lexer.h"
#include "parser.h"

#define MAX_STACK_SIZE 1024
typedef struct {
  int stack[MAX_STACK_SIZE];
  int stack_size; // not sure if it should be int or size_t
  size_t program_size;
  INST *instructions;
} Machine;

void rt_err(const char *msg) {
  fprintf(stderr, "RUNTIME ERROR: %s\n", msg);
  die(msg);
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
  fwrite(machine->instructions, sizeof(machine->instructions[0]),
         machine->program_size, file);
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
void push(Machine *machine, int val) {
  LOG2("LOG: push@(%d): %d\n", machine->stack_size, val);
  if (machine->stack_size >= MAX_STACK_SIZE)
    rt_err("stack overflow! (push)");

  machine->stack[machine->stack_size] = val;
  machine->stack_size++;
}

int pop(Machine *machine) {
  LOG2("LOG: pop\n");
  if (machine->stack_size <= 0)
    rt_err("stack underflow! (pop)");

  machine->stack_size--;
  return machine->stack[machine->stack_size];
}

int peek(Machine *machine, int depth) {
  if (depth > machine->stack_size)
    rt_err("tried to peek into the void (peek)");
  return machine->stack[machine->stack_size - depth - 1];
}

void idup(Machine *machine, int index) {
  if (index > machine->stack_size && index < 0) {
    rt_err("index out of range (swap)");
    exit(1);
  };
  push(machine, machine->stack[index]);
}

void iswap(Machine *machine, int index) {
  if (index > machine->stack_size && index < 0) {
    rt_err("index out of range (swap)");
    exit(1);
  };
  int tmp = machine->stack[index];
  machine->stack[index] = pop(machine);
  push(machine, tmp);
}

//
// running the thing
//

void execute_loop(Machine *m) {
  int a, b;
  for (size_t ip = 0; ip < m->program_size; ip++) {
    LOG2("#%u\n", m->instructions[ip].type);
    LOG2("INST %s, %d\n", inst_to_string(m->instructions[ip].type),
         m->instructions[ip].val);
    switch (m->instructions[ip].type) {
    case INST_NOP:
      continue;
      break;
    case INST_PUSH:
      push(m, m->instructions[ip].val);
      break;
    case INST_POP:
      pop(m);
      break;
    case INST_ADD: {
      int rhs = pop(m);
      int lhs = pop(m);
      push(m, lhs + rhs);
      break;
    }
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
    case INST_MUL: {
      a = pop(m);
      b = pop(m);
      push(m, a * b);
      break;
    }
    case INST_DIV: {
      int rhs = pop(m);
      int lhs = pop(m);
      if (rhs == 0)
        rt_err("cannot divide by 0");
      push(m, lhs / rhs);
      break;
    }
    case INST_SWAP:
      a = pop(m);
      b = pop(m);
      push(m, a);
      push(m, b);
      break;
    case INST_PRINTC:
      a = pop(m);
      printf("%c", a); // TODO: abstract
      break;
    case INST_PRINT:
      a = pop(m);
      printf("%d", a); // TODO: abstract
      break;
    case INST_HALT:
      exit(0);
      break;
    case INST_CMPE:
      push(m, peek(m, 0) == peek(m, 1));
      break;
    case INST_CMPL:
      push(m, peek(m, 0) < peek(m, 1));
      break;
    case INST_CMPG:
      push(m, peek(m, 0) > peek(m, 1));
      break;
    case INST_JMPZ:
      if (pop(m) == 0) {
        ip = m->instructions[ip].val - 1;
      } else
        continue;
      break;
    case INST_JPMNZ:
      if (pop(m) != 0) {
        ip = m->instructions[ip].val - 1;
      } else
        continue;
      break;
      ;
    case INST_CMPGE:
      push(m, peek(m, 0) >= peek(m, 1));
      break;
    case INST_CMPLE:
      push(m, peek(m, 0) <= peek(m, 1));
      break;
    case INST_JMP:
      ip = m->instructions[ip].val - 1;
      break;
    case INST_MOD: {
      int rhs = pop(m);
      int lhs = pop(m);
      if (rhs == 0)
        rt_err("cannot divide by 0");
      push(m, lhs / rhs);
      break;
    }
    case INST_INSWAP:
      iswap(m, m->instructions[ip].val);
      break;
    case INST_INDUP:
      idup(m, m->instructions[ip].val);
      break;
    case INST_NONE:
      die("tried executing INST_NONE");
      break;
    case INST_POW:
      int rhs = pop(m);
      int lhs = pop(m);
      if (rhs == 0)
        push(m, 1);
      push(m, pow(lhs, rhs));
      break;
    case INST_OR:
      push(m, peek(m, 0) || peek(m, 1));
      break;
    case INST_XOR:
      push(m, peek(m, 0) ^ peek(m, 1));
      break;
    case INST_AND:
      push(m, peek(m, 0) && peek(m, 1));
      break;
    }
  };
}

int main(int argc, char *argv[]) {
  Machine *machine = malloc(sizeof(Machine));
  if (argc > 0) {
    // printf("%s\n", argv[1]);
    int prog_size;
    machine->instructions = parser(lexer(argv[1]), &prog_size);
    machine->program_size = prog_size;
  }
  LOG2("did lex parse\n");
  // #ifdef INDEV
  //   machine->instructions = program;
  //   machine->program_size = PROGRAM_SIZE;
  //   prog_write_to_file(machine, "./prog.ab");
  // #else
  //   prog_read_from_file(m, "./prog.ab");
  // #endif /* ifdef INDEV */

  execute_loop(machine);

  LOG2("executed\n");
#if (LOG_LEVEL >= 1)
  print_stack(machine);
#endif
  return 0;
}
