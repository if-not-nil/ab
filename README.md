# a-b vm
a stack-based VM in C. i'm trying to leave coherent comments throughout, so it should be easy to read

for compile instructions look through the makefile. you may want to change your compiler

goals:
    - [ ] implement a good enough instructions set
    - [x] write an assembler
    - [ ] maybe switch over to c++?
    - [ ] labels in asm

the assembly looks like

```asm
push 5
pop
push 6
print
push 7
push 8
swap
dup
```

and has the instruction set of 

```asm
  INST_NOP = 0x00,  // no operation
  INST_PUSH = 0x01, // push a number (x) onto the stack
  INST_JMP = 0x02,  // jump to an instruction at x
  INST_SWAP = 0x03, // pop a then b, push b then a
  //
  // instructions with an index operator (x)
  INST_INSWAP = 0x04, // swap the top of the stack with a value at x
  INST_INDUP = 0x05,  // push a value which is at x to the top of the stack
  //
  // pop, one number, then...
  INST_PRINT = 0x06, // print it
  INST_JMPZ = 0x07,  // if it's 1, jump to an instruction at x
  INST_JPMNZ = 0x08, // if it's not 1, jump to an instruction at x
  INST_POP = 0x09,   // guess
  INST_DUP = 0x0A,   // push it twice
  //
  // arithmetic: pop two numbers, compute, then push one
  INST_ADD = 0x10, // a + b
  INST_MOD = 0x11, // a % b
  INST_SUB = 0x12, // a - b
  INST_MUL = 0x13, // a * b
  INST_DIV = 0x14, // a / b
  //
  // comparison: pop two numbers, compare, then push a number (1 or 0)
  INST_CMPE = 0x40,  // a == b
  INST_CMPL = 0x41,  // a < b
  INST_CMPG = 0x42,  // a > b
  INST_CMPGE = 0x43, // a > b
  INST_CMPLE = 0x44, // a > b
  //
  // special
  INST_HALT = 0xFF, // halt
  INST_NONE = -1,   // for lexer errors
```
