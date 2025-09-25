#include "instructions.hpp"
#include "lexer.hpp"
#include "machine.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "usage: ab <mode> <file>\n";
    return 1;
  }

  std::string mode = argv[1];
  std::string file_path = argv[2];

  Machine machine(2048);

  if (mode == "load") {
    Lexer lexer(file_path);
    Parser parser(&lexer);

    machine.load(parser.instructions);

    prog_write_to_file(machine.program, "./prog.ab");

    machine.execute();
  } else if (mode == "run") {
    machine.program = prog_read_from_file("./prog.ab");
    machine.execute();
  } else {
    std::cerr << "unknown mode: " << mode << "\n";
    std::cerr << "usage: ab <load|run> <file>\n";
    return 1;
  }

  return 0;
}
