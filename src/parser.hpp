#pragma once

#include "lexer.hpp"
#include "machine.hpp"

typedef std::tuple<std::string, WORD> Label;

struct Parser {
  std::vector<Token> tokens;
  Program prog;

  Parser(Lexer *lexer) {
    // every program starts inside an implicit main
    prog.functions["main"] = {};
    parse_function_body(lexer->stack, 0, "main");
    // for (auto f : prog.functions) {
    //   std::cout << "===parsed function " << f.first << '\n';
    //   for (auto i : f.second)
    //     std::cout << i.to_string() << '\n';
    //   std::cout << "===end " << f.first << '\n';
    // }
  }

  size_t parse_function_body(const std::vector<Token> &tokens,
                             size_t start_index, const std::string &fn_name) {
    std::unordered_map<std::string, WORD> labels;
    auto &insts = prog.functions[fn_name];
    int ip = 0;
    for (size_t i = start_index; i < tokens.size(); i++) {
      const Token &tok = tokens[i];
      if (tok.type == TOK_FN_DEF_END)
        break;
      if (tok.type == TOK_LABEL_DEF) {
        labels[tok.text] = ip;
        continue;
      } else if (!(tok.type == TOK_LABEL_DEF || tok.type == TOK_FN_DEF ||
                   tok.type == TOK_FN_DEF_END))
        ip++;
    }

    for (size_t i = start_index; i < tokens.size(); i++) { // pass 2
      const Token &tok = tokens[i];

      // nested fn
      if (tok.type == TOK_FN_DEF) {
        if (i + 1 >= tokens.size())
          throw std::runtime_error("<func> without name");
        i++;
        Token name_tok = tokens[i];
        std::cout << "ran into a nested function " << name_tok.to_str() << '\n';
        if (name_tok.type != TOK_IDENT)
          throw std::runtime_error("<func> not followed by identifier");

        prog.functions[name_tok.text] = {};
        std::cout << "trying to parse a function from "
                  << tokens[i + 1].to_str() << '\n';
        i = parse_function_body(tokens, i + 1, name_tok.text);
        continue;
      }

      if (tok.type == TOK_FN_DEF_END)
        return i;

      if (tok.type == TOK_INT) {
        if (insts.empty())
          throw std::runtime_error("a number can't be the first token");
        insts.back().val = std::stoi(tok.text);
        continue;
      }

      if (tok.type == TOK_JMP || tok.type == TOK_JMPZ ||
          tok.type == TOK_JMPNZ || tok.type == TOK_CALL) {
        if (i + 1 >= tokens.size())
          throw std::runtime_error("no identifier after jump/call");

        const Token &ident = tokens[i + 1];
        if (ident.type != TOK_IDENT)
          throw std::runtime_error("invalid label after jump/call: " +
                                   ident.text);

        WORD val = 0;
        if (tok.type == TOK_CALL) {
          val = find_function_index(ident.text);
        } else {
          val = labels[ident.text];
        }

        if (token_to_inst(tok.type) == INST_NONE)
          std::cout << ip << ": jmp: got TOK_NONE " << tok.to_str() << '\n';
        insts.push_back({.type = token_to_inst(tok.type), .val = val});
        i++;
        ip++;
        continue;
      }

      if (is_meta(tok.type))
        std::cout << ip << ": got TOK_NONE" << tok.to_str() << '\n';
      else {
        insts.push_back({.type = token_to_inst(tok.type)});
      }
      ip++;
    }

    return tokens.size() - 1;
  }

  // WORD find_label_index(const std::string &name) const {
  //   for (const auto &l : labels)
  //     if (std::get<0>(l) == name)
  //       return std::get<1>(l);
  //   throw std::runtime_error("undefined label: " + name);
  // }

  bool is_meta(TokenType tok) { return (tok == TOK_LABEL_DEF); };
  WORD find_function_index(const std::string &name) const {
    WORD idx = 0;
    for (const auto &pair : prog.functions) {
      if (pair.first == name)
        return idx;
      idx++;
    }
    throw std::runtime_error("undefined function: " + name);
  }
};
