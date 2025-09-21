#ifndef LEXER_H
#define LEXER_H
#include "common.h"
#include "instructions.h"
#include <ctype.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TOK_PUSH } TokenTypes;
typedef struct {
  INST_TYPE type;
  char *text;
  int line;
  int character;
} Token;

Token token_init(int line, int character, char *text, INST_TYPE type) {
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

INST_TYPE keyword_check_builtin(char *name) {
  if (strcmp(name, "push") == 0)
    return INST_PUSH;
  else if (strcmp(name, "pop") == 0)
    return INST_POP;
  else if (strcmp(name, "swap") == 0)
    return INST_SWAP;
  else if (strcmp(name, "dup") == 0)
    return INST_DUP;
  else if (strcmp(name, "nop") == 0)
    return INST_NOP;
  else if (strcmp(name, "push") == 0)
    return INST_PUSH;
  else if (strcmp(name, "jmp") == 0)
    return INST_JMP;
  else if (strcmp(name, "swap") == 0)
    return INST_SWAP;
  else if (strcmp(name, "inswap") == 0)
    return INST_INSWAP;
  else if (strcmp(name, "indup") == 0)
    return INST_INDUP;
  else if (strcmp(name, "print") == 0)
    return INST_PRINT;
  else if (strcmp(name, "jmpz") == 0)
    return INST_JMPZ;
  else if (strcmp(name, "jpmnz") == 0)
    return INST_JPMNZ;
  else if (strcmp(name, "pop") == 0)
    return INST_POP;
  else if (strcmp(name, "dup") == 0)
    return INST_DUP;
  else if (strcmp(name, "add") == 0)
    return INST_ADD;
  else if (strcmp(name, "mod") == 0)
    return INST_MOD;
  else if (strcmp(name, "sub") == 0)
    return INST_SUB;
  else if (strcmp(name, "mul") == 0)
    return INST_MUL;
  else if (strcmp(name, "div") == 0)
    return INST_DIV;
  else if (strcmp(name, "cmpe") == 0)
    return INST_CMPE;
  else if (strcmp(name, "cmpl") == 0)
    return INST_CMPL;
  else if (strcmp(name, "cmpg") == 0)
    return INST_CMPG;
  else if (strcmp(name, "cmpge") == 0)
    return INST_CMPGE;
  else if (strcmp(name, "cmple") == 0)
    return INST_CMPLE;
  else if (strcmp(name, "halt") == 0)
    return INST_HALT;
  else if (strcmp(name, "none") == 0)
    return INST_NONE;

  return -1;
}

Token keyword_generate(char *cur, int *idx, int line, int character) {
  char *keyword = malloc(sizeof(char) * 8);
  size_t keyword_len = 0;
  while (isalpha(cur[*idx])) {
    keyword[keyword_len] = cur[*idx];
    *idx += 1;
    keyword_len++;
  }
  keyword[keyword_len] = '\0';
  INST_TYPE type = keyword_check_builtin(keyword);
  chkdie(((int)type == -1), "couldnt parse token");
  Token token = token_init(line, character, keyword, type);
  return token;
}

int lexer(void) {
  int len = 0;
  char *cur = open_file("./prog.aba", &len);
  int idx = 0;

  int pos_line = 0;
  int pos_char = 0;

  // printf("%s\n", cur);
  while (idx < len) {
    char c = *(cur + idx);

    if (c == '\n') {
      pos_line++;
      pos_char++;
    }
    if (isalpha(c)) {
      Token tok = keyword_generate(cur, &idx, pos_line, pos_char);
      printf("tok %s @ %d:%d\n", tok.text, tok.line, tok.character);
    } else if (isdigit(c)) { // idk how it differs from isnumber()
      // printf("%c", c);
    }

    idx++;
  }
  return 0;
}

#endif // !LEXER_H
