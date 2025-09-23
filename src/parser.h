#ifndef INCLUDE_SRC_PARSER_H_
#define INCLUDE_SRC_PARSER_H_

#include "lexer.h"

typedef struct ParseList {
  Token val;
  struct ParseList *next;
} ParseList;

void pl_append(ParseList *head, Token val) {
  while (head->next != NULL)
    head = head->next;

  ParseList *new_node = malloc(sizeof(ParseList));
  chkdie(!new_node, "malloc failed for ParseList");

  new_node->val = val;
  new_node->next = NULL;

  head->next = new_node;
}

void pl_print(ParseList *head) {
  while (head != NULL) {
    token_print(head->val);
    head = head->next;
  }
}

#define MAX_PROGRAM_SIZE 1024
INST *parser(Lexer *lexer, int *program_size) {
  ParseList *root = malloc(sizeof(ParseList));
  chkdie(!root, "malloc failed for ParseList root");
  root->next = NULL;

  for (int i = 0; i < lexer->stack_size; i++) {
    pl_append(root, lexer->stack[i]);
  }
  pl_print(root);

  INST *insts = (INST *)malloc(sizeof(INST) * MAX_PROGRAM_SIZE);
  int inst_p = 0;

  for (ParseList *cur = root->next; cur != NULL; cur = cur->next) {
    TokenType type = cur->val.type;
    // the 0xDx range is for token specific stuff
    if (type >= 0xD0 && type < 0xE0) {
      insts[inst_p - 1].val = atoi(cur->val.text);
      if (isalpha(*cur->val.text))
        insts[inst_p - 1].val = (char)*cur->val.text;
      continue;
    }
    if (type != TOK_NOP) {
      INST inst = {.type = token_to_inst(type)};
      insts[inst_p] = inst;
    }
    inst_p++;
  }

#if LOG_LEVEL > 1
  printf("=== parser stack\n");
  for (int i = 0; i < inst_p; i++) {
    printf("%u: %d\n", insts[i].type, insts[i].val);
  }
#endif
  *program_size = inst_p;
  return insts;
}

#endif // INCLUDE_SRC_PARSER_H_
