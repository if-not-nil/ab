#ifndef INCLUDE_SRC_PARSER_H_
#define INCLUDE_SRC_PARSER_H_

#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ParseList {
  Token value;
  struct ParseList *next;
} ParseList;

void pl_append(ParseList *head, Token val) {
  while (head->next != NULL)
    head = head->next;

  ParseList *new_node = malloc(sizeof(ParseList));
  chkdie(!new_node, "malloc failed for ParseList");

  new_node->value = val;
  new_node->next = NULL;

  head->next = new_node;
}

#define MAX_PROGRAM_SIZE 1024
INST *parser(Lexer *lexer, int *program_size) {
  ParseList *root = malloc(sizeof(ParseList));
  chkdie(!root, "malloc failed for ParseList root");
  root->next = NULL;

  for (int i = 0; i < lexer->stack_size; i++) {
    pl_append(root, lexer->token_stack[i]);
  }

  INST *insts = (INST *)malloc(sizeof(INST) * MAX_PROGRAM_SIZE);
  int inst_p = 0;

  for (ParseList *cur = root->next; cur != NULL; cur = cur->next) {
    if (cur->value.type >= 0xD0 && cur->value.type < 0xE0) {
      // the 0xDx range is for token specific stuff
      insts[inst_p - 1].val = atoi(cur->value.text);
      if (isalpha(*cur->value.text))
        insts[inst_p - 1].val = (char)*cur->value.text;
      continue;
    }
    if (cur->value.type != TOK_NOP) {
      INST inst = {.type =
                       (int)(cur->value.type)}; // TODO: fix implicit conversion
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
