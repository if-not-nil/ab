#pragma once

#include "instructions.hpp"
#include "memory.hpp"

#include <cstddef>
#include <vector>

#define MAX_STACK_SIZE 1024

struct Machine {
  std::vector<uint8_t> stack;
  size_t stack_size;
  size_t program_size;
  Memory memory;
  std::vector<Inst> program;

  Machine(std::vector<Inst> program) : program(program) {
  };
};
