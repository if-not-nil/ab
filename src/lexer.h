#ifndef LEXER_H
#define LEXER_H
#include "common.h"
#include "instructions.h"
#include <ctype.h>

typedef struct {
  TokenType type;
  char *text;
  int line;
  int character;
} Token;

#define MAX_TOKEN_STACK_SIZE 512
typedef struct {
  Token stack[MAX_TOKEN_STACK_SIZE];
  int stack_size;
  // char *file_name;
} Lexer;

Token lexer_pop(Lexer *l) {
  if (l->stack_size < 0)
    die("LEXER: stack underflow");
  l->stack_size--;
  return l->stack[l->stack_size];
}

void lexer_push(Lexer *l, Token tok) {
  if (l->stack_size >= MAX_TOKEN_STACK_SIZE)
    die("LEXER: stack overflow");
  l->stack[l->stack_size] = tok;
  l->stack_size++;
}

Token token_init(int line, int character, char *text, TokenType type) {
  Token token = {
      .type = type, .line = line, .character = character, .text = text};
  return token;
}

void token_print(Token tok) {
  if (&tok + 0) // to silence the compiler
    ;
  LOG1("TOK %s: %s", token_to_string(tok.type), tok.text);
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
  char *keyword = malloc(sizeof(char) * 32);
  size_t keyword_len = 0;
  while (isalpha(cur[*idx]) || cur[*idx] == '_' || cur[*idx] == '.') {
    keyword[keyword_len] = cur[*idx];
    *idx += 1;
    keyword_len++;
  }
  keyword[keyword_len] = '\0';
  TokenType type = keyword_to_token(keyword);
  // chkdie(((int)type == TOK_NONE), "couldnt lex token");
  if (type == TOK_IDENT && cur[*idx] == ':') {
    type = TOK_LABEL_DEF;
    *idx += 1;
  }
  Token token = token_init(line, character, keyword, type);
  return token;
}

void lexer_print_stack(Lexer *lexer) {
  printf("\n=== lexer stack size %d:\n", lexer->stack_size);
  for (int i = 0; i <= lexer->stack_size - 1; i++) {
    printf("%d ", lexer->stack[i].type);
  }
  printf("\n===\n");
}

Lexer *lexer(char *file_path) {
  int len = 0;
  char *cur = open_file(file_path, &len);
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
    if (c == ';') {
      while (c != '\n') {
        c = *(cur + idx);
        idx++;
        continue;
      }
    }
    if (isalpha(c) || c == '_' || c == '.') { // . is meta stuff
      Token tok = lexer_generate_keyword(cur, &idx, pos_line, pos_char);
      idx--; // walk back a bit so that we hit newline
      lexer_push(lexer, tok);
      LOG1("tok\t (%s)\t @ %d:%d", tok.text, tok.line, tok.character);
    } else if (isdigit(c)) { // idk how it differs from isnumber()
      Token tok = lexer_generate_int(cur, &idx, pos_line, pos_char);
      idx--;
      lexer_push(lexer, tok);
      LOG1("num\t (%s)\t @ %d:%d", tok.text, tok.line, tok.character);
    }
    pos_char++;
    idx++;
  }
#if LOG_LEVEL > 1
  lexer_print_stack(lexer);
#endif
  return lexer;
}

#endif // !LEXER_H
