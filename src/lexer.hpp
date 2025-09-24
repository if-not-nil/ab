#pragma once

#include "common.hpp"
#include "instructions.hpp"
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct Token {
  Token(int line, int character, std::string text, TokenType type)
      : type(type), text(text), line(line), character(character) {};
  std::string to_str() {
    return "TOK{type=" + std::string(token_to_string(type)) + ",text=" + text +
           ",line=" + std::to_string(line) +
           ",character=" + std::to_string(character) + "}\n";
  };
  TokenType type;
  std::string text;
  int line;
  int character;
};

// file contents as a string
inline std::string read_file(const std::string &path) {
  std::ifstream file(path, std::ios::in | std::ios::binary);
  if (!file.is_open())
    throw std::runtime_error("could not open file: " + path);

  std::ostringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

#define MAX_TOKEN_STACK_SIZE 512
struct Lexer {
private:
  std::string contents;
  std::ifstream file;

public:
  std::vector<Token> stack;
  int pos_line = 1, pos_char = 1;
  Lexer(std::string file_path) {
    contents = read_file(file_path);
    lex();
  }
  Token pop() {
    Token a = stack.back();
    stack.pop_back();
    return a;
  }
  void push(Token tok) { stack.push_back(tok); }
  void print_stack() {
    printf("\n=== lexer stack size %zu:\n", stack.size());
    for (auto item : stack) {
      printf("%d ", item.type);
    }
    printf("\n===\n");
  }
  void lex() {
    size_t idx = 0;
    while (idx < contents.size()) {
      char c = contents[idx];
      if (c == '\n') {
        pos_line++;
        pos_char = 1;
        idx++;
        continue;
      }

      if (c == ';') { // comment
        while (idx < contents.size() && contents[idx] != '\n')
          idx++;
        continue;
      }

      if (std::isalpha(c) || c == '_' || c == '.') {
        Token tok = generate_keyword(idx);
        stack.push_back(tok);
      } else if (std::isdigit(c)) {
        Token tok = generate_int(idx);
        stack.push_back(tok);
      }
      pos_char++;
      idx++;
    }
// #if LOG_LEVEL > 1
//     print_stack();
// #endif
  }

  Token generate_int(size_t &idx) {
    size_t start = idx;
    while (idx < contents.size() && std::isdigit(contents[idx]))
      idx++;
    std::string number = contents.substr(start, idx - start);
    Token tok(pos_line, pos_char, (char *)number.c_str(), TOK_INT);
    idx--;
    return tok;
  }

  Token generate_keyword(size_t &idx) {
    size_t start = idx;
    while (idx < contents.size() &&
           (std::isalpha(contents[idx]) || contents[idx] == '_' ||
            contents[idx] == '.')) {
      idx++;
    }
    std::string word = contents.substr(start, idx - start);
    TokenType type = keyword_to_token(word.c_str());
    if (type == TOK_IDENT && idx < contents.size() && contents[idx] == ':') {
      type = TOK_LABEL_DEF;
      idx++;
    }
    Token tok(pos_line, pos_char, (char *)word.c_str(), type);
    idx--;
    return tok;
  }
};
