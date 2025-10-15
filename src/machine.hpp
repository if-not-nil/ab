#pragma once

#include "instructions.hpp"
#include "memory.hpp"
#include "prelude.hpp"
#include <cstddef>
#include <map>
#include <unordered_map>

inline void prog_write_to_file(const std::vector<Inst> &program,
                               const std::string path) {
  std::ofstream file(path, std::ios::binary);
  if (!file.is_open())
    throw std::runtime_error("couldnt open file for writing? " + path);
  file.write(reinterpret_cast<const char *>(
                 program.data()), // `(const char*)val` is not used here, as it
                                  // may do three or four different casts and
                                  // give you bullshit
             program.size() * sizeof(Inst));
}

inline std::vector<Inst> prog_read_from_file(const std::string &path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file)
    throw std::runtime_error("could not open file for reading: " + path);

  auto size = file.tellg();
  file.seekg(0, std::ios::beg);

  if (size % sizeof(Inst) != 0)
    throw std::runtime_error("file size is not a multiple of Inst size");

  std::vector<Inst> program(size / sizeof(Inst));
  if (!file.read(reinterpret_cast<char *>(program.data()), size))
    throw std::runtime_error("failed to read program from file");

  return program;
}

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

// executes instructions when given
struct Machine {
  Stack stack;
  Stack ret_stack;
  size_t stack_size;
  Memory memory;
  WORD registers[REG_NUM];

  std::vector<Inst> program;
  std::unordered_map<WORD, WORD> functions;

  Machine(size_t mem_size) : stack(1024), ret_stack(1024), memory(mem_size) {};

  void load(std::vector<Inst> program) {
    this->program = program;
    registers[REG_IP] = 0;
  };
  void execute() {
    while (registers[REG_IP] < program.size()) {
      std::cout << inst_to_string(program[registers[REG_IP]].type) << ": "
                << program[registers[REG_IP]].val << '\n';
      execute_instruction(program[registers[REG_IP]]);
      registers[REG_IP]++;
    }
  };
  void execute_instruction(Inst inst) {
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
    case INST_RETURN:
      // FN_ADD: reg_fn = [0]
      //   add [1,2] -> 3 RS: [5]
      //   ret [jmp]      RS:
      //
      // push 1 [1]
      // push 2 [1,2]
      // call 0 [1,2] RS: [5] -> [3], []
      registers[REG_IP] = ret_stack.pop();
    case INST_FN_DEF:
      functions[(WORD)inst.val] = registers[REG_IP];
      break;
    case INST_CALL:
      ret_stack.push(registers[REG_IP]);
      registers[REG_IP] = functions.at(stack.pop());
      break;
    }

    // #if LOG_LEVEL > 2
    //     print_stack();
    // #endif
  }
};

;
