#ifndef INCLUDE_SRC_PARSER_H_
#define INCLUDE_SRC_PARSER_H_

#include "lexer.h"
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

ParseList *parser(Lexer *lexer) {
  ParseList *root = malloc(sizeof(ParseList));
  chkdie(!root, "malloc failed for ParseList root");
  root->next = NULL;

  for (int i = 0; i < lexer->stack_size; i++) {
    pl_append(root, lexer->token_stack[i]);
  }

  for (ParseList *cur = root->next; cur != NULL; cur = cur->next) {
    printf("%d\n", cur->value.type);
  }

  return root;
}

#endif // INCLUDE_SRC_PARSER_H_
