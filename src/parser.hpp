#pragma once

#include "prelude.hpp"
#include "lexer.hpp"

#define MAX_PROGRAM_SIZE 1024

typedef std::tuple<std::string, WORD> Label;

struct Macro { // theres no good reason to use a tuple for one and a struct for
               // another i just just gotta see tuple works
  std::string name;
  std::vector<Inst> instructions;
};

struct Parser {
  std::vector<Label> labels;
  std::vector<Inst> instructions;
  std::vector<Token> tokens;
  std::vector<Macro> macros;

  Parser(Lexer *lexer) {
    // first pass for labels
    WORD inst_i = 0;
    for (size_t i = 0; i < lexer->stack.size(); i++) {
      Token tok = lexer->stack[i];

      if (tok.type == TOK_LABEL_DEF) {
        labels.push_back({tok.text, inst_i});
      } else {
        tokens.push_back(Token(tok.line, tok.character, tok.text, tok.type));
        if (tok.type < 0xD0 || tok.type > 0xDF)
          inst_i++;
      }
    };
    // print_tokens();
    // second pass for everything else

    for (size_t i = 0; i < tokens.size(); i++) {
      Token tok = tokens[i];

      if (tok.type == TOK_INT) {
        if (i == 0)
          throw std::runtime_error("a number cant be the first token!");
        instructions.back().val = std::stoi(tok.text);
        continue;
      }

      if (tok.type == TOK_JMP || tok.type == TOK_JMPZ ||
          tok.type == TOK_JMPNZ) {
        if (i + 1 >= lexer->stack.size())
          throw std::runtime_error("no identifier after a jump instruction!");

        Token ident = lexer->stack[i + 2];
        if (ident.type != TOK_IDENT)
          throw std::runtime_error("whatever you have after the jump "
                                   "instruction isnt a valid label\n" +
                                   ident.to_str());

        auto it = // take c++ away from me bruh
            std::find_if(labels.begin(), labels.end(), [&](const Label &l) {
              return std::get<0>(l) == ident.text;
            });

        if (it == labels.end())
          throw std::runtime_error("undefined label " + ident.text);

        instructions.push_back(
            {.type = token_to_inst(tok.type), .val = std::get<1>(*it)});
        i++;
        continue;
      } // JMP TOKEN END
      instructions.push_back({.type = token_to_inst(tok.type)});
    };
  }
  void print_tokens() {
    for (auto it : tokens) {
      std::cout << it.to_str();
    }
  }
};
