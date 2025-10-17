#include "prelude.hpp"
// if defined, the program is the one at the bottom of this file. else loaded
// from prog.ab
#define INDEV
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

// later: make ranges good
//   0x0x for elementary
//   0x1x for arithmetic
//   0x4x for comparisons
#define INSTRUCTION_LIST                                                       \
  X(NONE, -1, "none")                                                          \
  X(NOP, 0x00, "nop")                                                          \
  X(PUSH, 0x01, "push")                                                        \
  X(JMP, 0x02, "jmp")                                                          \
  X(SWAP, 0x03, "swap")                                                        \
  X(INSWAP, 0x04, "inswap")                                                    \
  X(INDUP, 0x05, "indup")                                                      \
  X(PRINT, 0x50, "print")                                                      \
  X(PRINTC, 0x51, "printc")                                                    \
  X(JMPZ, 0x07, "jmpz")                                                        \
  X(JMPNZ, 0x08, "jmpnz")                                                      \
  X(POP, 0x09, "pop")                                                          \
  X(DUP, 0x0A, "dup")                                                          \
  X(OVER, 0x0B, "over")                                                        \
  X(ROT, 0x0C, "rot")                                                          \
  X(ADD, 0x10, "add")                                                          \
  X(MOD, 0x11, "mod")                                                          \
  X(SUB, 0x12, "sub")                                                          \
  X(MUL, 0x13, "mul")                                                          \
  X(DIV, 0x14, "div")                                                          \
  X(POW, 0x15, "pow")                                                          \
  X(NEG, 0x16, "neg")                                                          \
  X(CMPE, 0x40, "cmpe")                                                        \
  X(CMPL, 0x41, "cmpl")                                                        \
  X(CMPG, 0x42, "cmpg")                                                        \
  X(CMPGE, 0x43, "cmpge")                                                      \
  X(CMPLE, 0x44, "cmple")                                                      \
  X(OR, 0x45, "or")                                                            \
  X(XOR, 0x46, "xor")                                                          \
  X(AND, 0x47, "and")                                                          \
  X(NOT, 0x48, "not")                                                          \
  X(LOAD, 0xF0, "load")                                                        \
  X(STORE, 0xF1, "store")                                                      \
  X(HALT, 0xFF, "halt")                                                        \
  X(CALL, 0xE2, "call")                                                        \
  X(RETURN, 0xE3, "ret")

#define TOKEN_LIST                                                             \
  X(INT, 0xD1, "int")                                                          \
  X(CHAR, 0xD2, "char")                                                        \
  X(IDENT, 0xD3, "ident")                                                      \
  X(EOF, 0xD4, "eof")                                                          \
  X(ERROR, 0xD5, "error")                                                      \
  X(LABEL_DEF, 0xD6, "label_def")                                              \
  X(MACRO_START, 0xD7, ".macro")                                               \
  X(FN_DEF, 0xE0, "fn")                                                        \
  X(FN_DEF_END, 0xE1, "endfn")                                                 \
  X(MACRO_END, 0xD8, ".endm")

// instructions enum
typedef enum {
#define X(name, opcode, _) INST_##name = (opcode), // x-macros saved my life
  INSTRUCTION_LIST
#undef X
} InstType;

// lexer enum, instructions + special tokens
typedef enum {
#define X(name, opcode, _) TOK_##name = (opcode),
  INSTRUCTION_LIST TOKEN_LIST
#undef X
} TokenType;

static inline const char *inst_to_string(InstType inst) {
  switch (inst) {
#define X(name, opcode, str)                                                   \
  case INST_##name:                                                            \
    return (str);
    INSTRUCTION_LIST
#undef X
  default:
    return "unknown";
  }
}

struct Inst {
  InstType type;
  WORD val;
  std::string to_string() {
    return std::string(inst_to_string(type)) + " (" + std::to_string(val) +
           ")\n";
  }
};

static inline const char *token_to_string(TokenType tok) {
  switch (tok) {
#define X(name, opcode, str)                                                   \
  case TOK_##name:                                                             \
    return (str);
    INSTRUCTION_LIST
    TOKEN_LIST
#undef X
  default:
    return "unknown";
  }
}

static inline InstType token_to_inst(TokenType tok) {
  switch (tok) {
#define X(sym, opcode, str)                                                    \
  case TOK_##sym:                                                              \
    return INST_##sym;
    INSTRUCTION_LIST
#undef X
  default: {
    // std::cout << "ran into INST_NONE "<< token_to_string(tok) << '\n';
    return INST_NONE;
  }
  }
}

static inline TokenType keyword_to_token(const char *word) {
#define X(sym, opcode, str)                                                    \
  if (strcmp(word, (str)) == 0)                                                \
    return TOK_##sym;
  INSTRUCTION_LIST
  TOKEN_LIST
#undef X
  return TOK_IDENT;
}

#endif
