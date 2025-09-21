#ifndef LEXER_H
#define LEXER_H
#include "common.h"
#include "instructions.h"
#include <ctype.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TOK_NOP = 0x00,
  TOK_PUSH,
  TOK_JMP,
  TOK_SWAP,
  TOK_INSWAP,
  TOK_INDUP,
  TOK_PRINT,
  TOK_JMPZ,
  TOK_JPMNZ,
  TOK_POP,
  TOK_DUP,
  //
  // arithmetic: pop two numbers, compute, then push one
  TOK_ADD, // a + b
  TOK_MOD, // a % b
  TOK_SUB, // a - b
  TOK_MUL, // a * b
  TOK_DIV, // a / b
  //
  // comparison: pop two numbers, compare, then push a number (1 or 0)
  TOK_CMPE,  // a == b
  TOK_CMPL,  // a < b
  TOK_CMPG,  // a > b
  TOK_CMPGE, // a > b
  TOK_CMPLE, // a > b
  //
  // special
  TOK_HALT = 0xFF, // halt
  TOK_NONE,
  TOK_INT,
} TokenType;

typedef struct {
  TokenType type;
  char *text;
  int line;
  int character;
} Token;

#define MAX_TOKEN_STACK_SIZE 512
typedef struct {
  Token token_stack[MAX_TOKEN_STACK_SIZE];
  int stack_size;
  // char *file_name;
} Lexer;

Token lexer_pop(Lexer *l) {
  if (l->stack_size < 0)
    die("LEXER: stack underflow");
  l->stack_size--;
  return l->token_stack[l->stack_size];
}
void lexer_push(Lexer *l, Token tok) {
  if (l->stack_size >= MAX_TOKEN_STACK_SIZE)
    die("LEXER: stack overflow");
  l->token_stack[l->stack_size] = tok;
  l->stack_size++;
}

Token token_init(int line, int character, char *text, TokenType type) {
  Token token = {
      .type = type, .line = line, .character = character, .text = text};
  return token;
}

char *open_file(char *path, int *len) {
  char *cur;
  FILE *file;
  file = fopen(path, "r");
  chkdie(!file, "couldnt open file");

  fseek(file, 0, SEEK_END);
  *len = ftell(file);
  fseek(file, 0, SEEK_SET);

  cur = malloc(sizeof(char) * *len);
  chkdie(!cur, "couldnt read from file");
  fread(cur, 1, *len, file);
  fclose(file);
  return cur;
}

TokenType keyword_check_builtin(char *name) {
  if (strcmp(name, "push") == 0)
    return TOK_PUSH;
  else if (strcmp(name, "pop") == 0)
    return TOK_POP;
  else if (strcmp(name, "swap") == 0)
    return TOK_SWAP;
  else if (strcmp(name, "dup") == 0)
    return TOK_DUP;
  else if (strcmp(name, "nop") == 0)
    return TOK_NOP;
  else if (strcmp(name, "push") == 0)
    return TOK_PUSH;
  else if (strcmp(name, "jmp") == 0)
    return TOK_JMP;
  else if (strcmp(name, "swap") == 0)
    return TOK_SWAP;
  else if (strcmp(name, "inswap") == 0)
    return TOK_INSWAP;
  else if (strcmp(name, "indup") == 0)
    return TOK_INDUP;
  else if (strcmp(name, "print") == 0)
    return TOK_PRINT;
  else if (strcmp(name, "jmpz") == 0)
    return TOK_JMPZ;
  else if (strcmp(name, "jpmnz") == 0)
    return TOK_JPMNZ;
  else if (strcmp(name, "pop") == 0)
    return TOK_POP;
  else if (strcmp(name, "dup") == 0)
    return TOK_DUP;
  else if (strcmp(name, "add") == 0)
    return TOK_ADD;
  else if (strcmp(name, "mod") == 0)
    return TOK_MOD;
  else if (strcmp(name, "sub") == 0)
    return TOK_SUB;
  else if (strcmp(name, "mul") == 0)
    return TOK_MUL;
  else if (strcmp(name, "div") == 0)
    return TOK_DIV;
  else if (strcmp(name, "cmpe") == 0)
    return TOK_CMPE;
  else if (strcmp(name, "cmpl") == 0)
    return TOK_CMPL;
  else if (strcmp(name, "cmpg") == 0)
    return TOK_CMPG;
  else if (strcmp(name, "cmpge") == 0)
    return TOK_CMPGE;
  else if (strcmp(name, "cmple") == 0)
    return TOK_CMPLE;
  else if (strcmp(name, "halt") == 0)
    return TOK_HALT;
  else if (strcmp(name, "none") == 0)
    return TOK_NONE;

  return -1;
}

Token lexer_generate_int(char *cur, int *idx, int line, int character) {
  char *keyword = malloc(sizeof(char) * 8);
  size_t keyword_len = 0;
  while (isdigit(cur[*idx])) {
    keyword[keyword_len] = cur[*idx];
    *idx += 1;
    keyword_len++;
  }
  keyword[keyword_len] = '\0';
  // atoi(keyword) somewhere here later maybe
  TokenType type = TOK_INT;
  Token token = token_init(line, character, keyword, type);
  return token;
}

Token lexer_generate_keyword(char *cur, int *idx, int line, int character) {
  char *keyword = malloc(sizeof(char) * 8);
  size_t keyword_len = 0;
  while (isalpha(cur[*idx])) {
    keyword[keyword_len] = cur[*idx];
    *idx += 1;
    keyword_len++;
  }
  keyword[keyword_len] = '\0';
  TokenType type = keyword_check_builtin(keyword);
  chkdie(((int)type == TOK_NONE), "couldnt lex token");
  Token token = token_init(line, character, keyword, type);
  return token;
}

void lexer_print_stack(Lexer *machine) {
  printf("\n=== lexer stack size %d:\n", machine->stack_size);
  for (int i = 0; i <= machine->stack_size - 1; i++) {
    printf("%d ", machine->token_stack[i].type);
  }
  printf("\n===\n");
}
Lexer *lexer(void) {
  int len = 0;
  char *cur = open_file("./prog.aba", &len);
  int idx = 0;

  Lexer *lexer = malloc(sizeof(Lexer));
  int pos_line = 1;
  int pos_char = 1;

  // printf("%s\n", cur);
  while (idx < len) {
    char c = *(cur + idx);
    if (c == '\n' || c == '\r') {
      pos_line++;
      pos_char = 1;
      idx++;
      continue;
    }
    if (isalpha(c)) {
      Token tok = lexer_generate_keyword(cur, &idx, pos_line, pos_char);
      idx--; // walk back a bit so that we hit newline
      lexer_push(lexer, tok);
      printf("tok\t (%s)\t @ %d:%d\n", tok.text, tok.line, tok.character);
    } else if (isdigit(c)) { // idk how it differs from isnumber()
      Token tok = lexer_generate_int(cur, &idx, pos_line, pos_char);
      idx--;
      lexer_push(lexer, tok);
      printf("num\t (%s)\t @ %d:%d\n", tok.text, tok.line, tok.character);
    }
    pos_char++;
    idx++;
  }
  lexer_print_stack(lexer);
  return lexer;
}

#endif // !LEXER_H
