#pragma once

#include "prelude.hpp"


struct Memory {
  std::vector<WORD> data;
  // look into the `explicit` keyword. it makes it so that you can do Memory memory(2048); but not Memory memory = 2048;
  Memory(size_t size)
      : data(size, 0) {}; // same as doing `data = std::vector<WORD>(size, 0);`!
                          // insane what this language can do

  template <typename T> void store(size_t pos, T value) {
    if (pos + sizeof(T) > data.size())
      throw std::runtime_error("cant park there sir");

    std::memcpy(data.data() + pos, &value, sizeof(T));
  }
  // should've seen how horrible this looked before c++
  template <typename T> T load(size_t pos) {
    if (pos + sizeof(T) > data.size())
      throw std::runtime_error("loaded beyond the memory size");

    T val{};
    std::memcpy(&val, data.data() + pos, sizeof(T));
    return val;
  }
};
