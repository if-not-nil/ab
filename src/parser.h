#ifndef INCLUDE_SRC_PARSER_H_
#define INCLUDE_SRC_PARSER_H_

#include "common.h"
#include "lexer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct {
  char *name;
  int addr;
} Label;

#define MAX_PROGRAM_SIZE 1024
// TODO: make the parselist an array and the label array a linked list
INST *parser(Lexer *lexer, int *program_size) {
  ParseList *root = malloc(sizeof(ParseList));
  chkdie(!root, "malloc failed for ParseList root");
  root->next = NULL;
  Label *labels = malloc(sizeof(Label) * 16);
  int label_count = 0;
  int inst_index = 0;

  for (int i = 0; i < lexer->stack_size; i++) {
    Token tok = lexer->stack[i];
    if (tok.type == TOK_LABEL_DEF) {
      labels[label_count].name = strdup(tok.text);
      labels[label_count].addr = inst_index;
      LOG2("found label %s, %d", labels[label_count].name,
           labels[label_count].addr);
      label_count++;
    } else {
      pl_append(root, tok);
      if (token_to_inst(tok.type) != INST_NONE)
        inst_index++;
    }
  }

  pl_print(root);

  INST *insts = (INST *)malloc(sizeof(INST) * MAX_PROGRAM_SIZE);
  int inst_p = 0;

  for (ParseList *cur = root->next; cur != NULL; cur = cur->next) {
    TokenType type = cur->val.type;
    // LOG2("GOT %s\n", token_to_string(type));
    // the 0xDx range is for token specific stuff
    if (type == TOK_INT) {
      chkdie(inst_p == 0, "INT token b4 a preceding one");
      insts[inst_p - 1].val = atoi(cur->val.text);
      continue;
    }
    INST inst = {.type = token_to_inst(type)};
    if (type == TOK_JMP || type == TOK_JMPZ || type == TOK_JMPNZ) {
      Token ident = cur->next->val;
      assert(ident.type == TOK_IDENT);
      int found = 0;

      for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name, ident.text) == 0) {
          inst.val = labels[i].addr;
          found = 1;
          break;
        }
      }

      if (!found) {
        die("undefined label! %s", ident.text);
      }
      cur = cur->next; // skip the TOK_IDENT
    }
    LOG2("ended up with inst %s val %d", inst_to_string(inst.type), inst.val);
    insts[inst_p] = inst;
    inst_p++;
  }

#if LOG_LEVEL > 1
  printf("=== parser stack\n");
  for (int i = 0; i < inst_p; i++) {
    printf("%s: %d\n", inst_to_string(insts[i].type), insts[i].val);
  }
  printf("===\n");
#endif

  *program_size = inst_p;
  ParseList *cur = root;
  while (cur) {
    ParseList *next = cur->next;
    free(cur);
    cur = next;
  }
  for (int i = 0; i < label_count; i++) {
    free(labels[i].name); // bc i did strdup
  }
  free(labels);
  return insts;
}

#endif // INCLUDE_SRC_PARSER_H_
