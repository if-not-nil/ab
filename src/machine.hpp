#pragma once

#include "instructions.hpp"
#include "memory.hpp"
#include "prelude.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <unordered_map>

class Stack {
public:
  explicit Stack(std::size_t max_size) : max_size(max_size) {}

  void push(WORD val) {
    if (data.size() >= max_size)
      throw std::runtime_error("stack overflow! (push)");
    data.push_back(val);
    // std::cout << "push@(" << data.size() - 1 << "): " << val << "\n";
  }

  WORD pop() {
    if (data.empty())
      throw std::runtime_error("stack underflow! (pop)");
    WORD val = data.back();
    data.pop_back();
    // std::cout << "pop\n";
    return val;
  }

  WORD peek(std::size_t depth = 0) const {
    if (depth >= data.size())
      throw std::runtime_error("tried to peek into the void (peek)");
    WORD val = data[data.size() - depth - 1];
    // std::cout << "peek returned " << val << "\n";
    return val;
  }

  void dup(std::size_t index) {
    if (index >= data.size())
      throw std::runtime_error("index out of range (dup)");
    push(data[index]);
  }

  void swap(std::size_t index) {
    if (index >= data.size())
      throw std::runtime_error("index out of range (swap)");
    WORD tmp = data[index];
    data[index] = pop();
    push(tmp);
  }

  void resize(size_t size) { data.resize(size); }
  std::size_t size() const { return data.size(); }

private:
  std::vector<WORD> data;
  std::size_t max_size;
};

enum Registers { REG_IP, REG_NUM };

struct Program {
  // std::vector<std::tuple<std::string, std::vector<Inst>>> functions;
  std::map<std::string, std::vector<Inst>> functions;
  void write_to_file(std::string path) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
      throw std::runtime_error("couldnt open file for writing? " + path);
    size_t fn_count = functions.size();
    file.write(reinterpret_cast<const char *>(&fn_count), sizeof(fn_count));

    for (const auto &fn : functions) {
      auto name = fn.first;
      auto insts = fn.second;
      size_t name_len = name.size();

      file.write(reinterpret_cast<const char *>(&name_len), sizeof(name_len));
      file.write(name.data(), name_len);

      size_t inst_count = insts.size();
      file.write(reinterpret_cast<const char *>(&inst_count),
                 sizeof(inst_count));

      file.write(reinterpret_cast<const char *>(insts.data()),
                 inst_count * sizeof(Inst));
    }
    file.close();
  }

  void read_from_file(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
      throw std::runtime_error("couldnt open file for reading? " + path);
    functions.clear();

    size_t fn_count = 0;
    file.read(reinterpret_cast<char *>(&fn_count), sizeof(fn_count));

    for (size_t i = 0; i < fn_count; ++i) {
      size_t name_len = 0;
      file.read(reinterpret_cast<char *>(&name_len), sizeof(name_len));
      std::string name(name_len, '\0');
      file.read((char *)name.data(),
                name_len); // idk why it didnt let me withuot converting it

      size_t inst_count = 0;
      file.read(reinterpret_cast<char *>(&inst_count), sizeof(inst_count));
      std::vector<Inst> insts(inst_count);
      file.read(reinterpret_cast<char *>(insts.data()),
                inst_count * sizeof(Inst));

      functions.emplace(name, std::move(insts));
    }
  }
};

struct ReturnFrame {
  std::string fn_name;
  WORD ip;
};

// executes instructions when given
struct Machine {
  Stack stack;
  size_t stack_size;
  Memory memory;
  Program program;
  WORD registers[REG_NUM];
  std::string cur_fn;
  std::vector<ReturnFrame> call_stack;

  Machine(size_t mem_size) : stack(1024), memory(mem_size) {};

  void load(Program prog) {
    this->program = prog;
    this->registers[REG_IP] = 0;
  }

  void execute(std::string function) {
    cur_fn = function;
    registers[REG_IP] = 0;

    while (true) {
      auto &insts = program.functions.at(cur_fn);

      if (registers[REG_IP] >= insts.size())
        break;

      execute_instruction(insts[registers[REG_IP]]);
      registers[REG_IP]++;
    }
  }
  void execute_instruction(Inst inst) {
    // std::cout << inst.to_string() << '\n';
    switch (inst.type) {
    case INST_NOP: {
      return;
      break;
    }
    case INST_PUSH: {
      stack.push(inst.val);
      break;
    }
    case INST_POP: {
      stack.pop();
      break;
    }
    case INST_ADD: {
      WORD rhs = stack.pop();
      WORD lhs = stack.pop();
      stack.push(lhs + rhs);
      break;
    }
    case INST_DUP:
      stack.push(stack.peek(0));
      break;
    case INST_SUB: {
      WORD a = stack.pop();
      WORD b = stack.pop();
      stack.push(b - a);
      break;
    }
    case INST_MUL: {
      WORD lhs = stack.pop();
      WORD rhs = stack.pop();
      stack.push(lhs * rhs);
      break;
    }
    case INST_DIV: {
      WORD rhs = stack.pop();
      WORD lhs = stack.pop();
      if (rhs == 0)
        throw std::runtime_error("cannot divide by 0");
      stack.push(lhs / rhs);
      break;
    }
    case INST_SWAP: {
      WORD a = stack.pop();
      WORD b = stack.pop();
      stack.push(a);
      stack.push(b);
      break;
    }
    case INST_PRINTC: {
      WORD a = stack.pop();
      printf("%c", a); // TODO: abstract
      break;
    }
    case INST_PRINT: {
      WORD a = stack.pop();
      printf("%d", a); // TODO: abstract
      break;
    }
    case INST_HALT: {
      exit(0);
      break;
    }
    case INST_CMPE: {
      WORD rhs = stack.pop();
      WORD lhs = stack.pop();
      stack.push(lhs == rhs);
      break;
    }
    case INST_CMPL: {
      WORD rhs = stack.pop();
      WORD lhs = stack.pop();
      stack.push(lhs < rhs);
      break;
    }
    case INST_CMPG: {
      WORD rhs = stack.pop();
      WORD lhs = stack.pop();
      stack.push(lhs > rhs);
      break;
    }
    case INST_JMPZ: {
      if (stack.pop() == 0) {
        registers[REG_IP] = inst.val - 1;
      } else
        return;
      break;
    }
    case INST_JMPNZ: {
      if (stack.pop() != 0) {
        registers[REG_IP] = inst.val - 1;
      } else
        return;
      break;
    }
    case INST_CMPGE: {
      WORD rhs = stack.pop();
      WORD lhs = stack.pop();
      stack.push(lhs >= rhs);
      break;
    }
    case INST_CMPLE: {
      WORD rhs = stack.pop();
      WORD lhs = stack.pop();
      stack.push(lhs <= rhs);
      break;
    }
    case INST_JMP: {
      registers[REG_IP] = inst.val - 1;
      break;
    }
    case INST_MOD: {
      WORD rhs = stack.pop();
      WORD lhs = stack.pop();
      if (rhs == 0)
        throw std::runtime_error("cannot divide by 0");
      stack.push(lhs / rhs);
      break;
    }
    case INST_INSWAP: {
      stack.swap(inst.val);
      break;
    }
    case INST_INDUP: {
      stack.dup(inst.val);
      break;
    }
    case INST_NONE: {
      throw std::runtime_error("tried executing INST_NONE");
      break;
    }
    case INST_POW: {
      WORD rhs = stack.pop();
      WORD lhs = stack.pop();
      if (rhs == 0)
        stack.push(1);
      stack.push(pow(lhs, rhs));
      break;
    }
    case INST_OR:
      stack.push(stack.peek(0) || stack.peek(1));
      break;
    case INST_XOR:
      stack.push(stack.peek(0) ^ stack.peek(1));
      break;
    case INST_AND:
      stack.push(stack.peek(0) && stack.peek(1));
      break;
    case INST_OVER:
      stack.push(stack.peek(1));
      break;
    case INST_ROT: {
      WORD c = stack.pop();
      WORD b = stack.pop();
      WORD a = stack.pop();
      stack.push(b);
      stack.push(c);
      stack.push(a);
      break;
    }
    case INST_NEG:
      stack.push(-(stack.pop()));
      break;
    case INST_NOT: {
      stack.push(~(stack.pop()));
      break;
    }
    case INST_LOAD:
      stack.push(memory.load<WORD>(inst.val));
      break;
    case INST_STORE:
      memory.store(stack.pop(), inst.val);
      break;
    case INST_RETURN: {
      if (call_stack.empty())
        throw std::runtime_error("return with empty call stack");
      auto frame = call_stack.back();
      call_stack.pop_back();
      cur_fn = frame.fn_name;
      registers[REG_IP] = frame.ip;
      break;
    }
    case INST_CALL: {
      call_stack.push_back({cur_fn, registers[REG_IP]});
      WORD target_idx = inst.val;

      auto it = program.functions.begin();
      std::advance(it, target_idx);
      cur_fn = it->first;
      registers[REG_IP] = -1; // its incremented to 0 right after breaking
      break;
    }
    }
  }
};

;
