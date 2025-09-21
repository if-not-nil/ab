// if defined, the program is the one at the bottom of this file. else loaded
// from prog.ab
#define INDEV
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

typedef enum {
  INST_NOP = 0x00, // nope
  INST_PUSH,       // push a number (x) onto the stack
  INST_JMP,        // jump to an instruction at x
  INST_SWAP,       // pop a then b, push b then a
  //
  // instructions with an index operator (x)
  INST_INSWAP,     // swap the top of the stack with a value at x
  INST_INDUP,      // push a value which is at x to the top of the stack
  //
  // pop, one number, then...
  INST_PRINT, // print it
  INST_JMPZ,  // if it's 1, jump to an instruction at x
  INST_JPMNZ, // if it's not 1, jump to an instruction at x
  INST_POP,   // guess
  INST_DUP,   // push it twice
  //
  // arithmetic: pop two numbers, compute, then push one
  INST_ADD, // a + b
  INST_MOD, // a % b
  INST_SUB, // a - b
  INST_MUL, // a * b
  INST_DIV, // a / b
  //
  // comparison: pop two numbers, compare, then push a number (1 or 0)
  INST_CMPE,  // a == b
  INST_CMPL,  // a < b
  INST_CMPG,  // a > b
  INST_CMPGE, // a > b
  INST_CMPLE, // a > b
  //
  // special
  INST_HALT = 0xFF, // halt
  INST_NONE = -1,   // for parser errors
} INST_TYPE;

typedef struct {
  INST_TYPE type;
  int val;
} INST;

#define DEF_INST_NOP {.type = INST_NOP}
#define DEF_INST_PUSH(x) {.type = INST_PUSH, .val = x}
#define DEF_INST_JMP(x) {.type = INST_JMP, .val = x}
#define DEF_INST_JMPNZ(x) {.type = INST_JMPNZ, .val = x}
#define DEF_INST_JMPZ(x) {.type = INST_JMPZ, .val = x}
#define DEF_INST_INSWAP(x) {.type = INST_INSWAP, .val = x}
#define DEF_INST_INDUP(x) {.type = INST_INDUP, .val = x}
#define DEF_INST_PRINT {.type = INST_PRINT}
#define DEF_INST_SWAP {.type = INST_SWAP}
#define DEF_INST_CMPE {.type = INST_CMPE}
#define DEF_INST_CMPGE {.type = INST_CMPGE}
#define DEF_INST_CMPLE {.type = INST_CMPLE}
#define DEF_INST_CMPL {.type = INST_CMPL}
#define DEF_INST_CMPG {.type = INST_CMPG}
#define DEF_INST_POP {.type = INST_POP}
#define DEF_INST_ADD {.type = INST_ADD}
#define DEF_INST_MOD {.type = INST_MOD}
#define DEF_INST_SUB {.type = INST_SUB}
#define DEF_INST_MUL {.type = INST_MUL}
#define DEF_INST_DUP {.type = INST_DUP}
#define DEF_INST_DIV {.type = INST_DIV}
#define DEF_INST_HALT {.type = INST_HALT}

#ifdef INDEV

INST program[] = {DEF_INST_PUSH(1), DEF_INST_PUSH(2), DEF_INST_PUSH(3),
                  DEF_INST_PUSH(4), DEF_INST_PUSH(5), DEF_INST_INSWAP(2)};

#define PROGRAM_SIZE (sizeof(program) / sizeof(program[0]))

#endif // INDEV
#endif // INSTRUCTIONS_H
