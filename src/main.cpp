#include "instructions.hpp"
#include "lexer.hpp"
#include "machine.hpp"
#include "parser.hpp"
#include "prelude.hpp"

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

    machine.load(parser.prog);

    // machine.program.write_to_file("./prog.ab");

    machine.execute("main");
  } else if (mode == "run") {
    // machine.program.read_from_file("./prog.ab");
    machine.execute("main");
  } else {
    std::cerr << "unknown mode: " << mode << "\n";
    std::cerr << "usage: ab <load|run> <file>\n";
    return 1;
  }

  return 0;
}
