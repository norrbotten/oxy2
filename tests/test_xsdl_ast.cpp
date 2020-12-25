#include "renderer/xsdl/xsdl.hpp"

#include <iostream>

int main() {
  using namespace Oxy::XSDL;

  {
    Compiler::Parser parser("5");
    if (parser.match_expression()) {
      std::cout << parser.ast()->dump();
    }
  }

  std::cout << "\n";

  {
    Compiler::Parser parser("5.0");
    if (parser.match_expression()) {
      std::cout << parser.ast()->dump();
    }
  }

  std::cout << "\n";

  {
    Compiler::Parser parser("abc");
    if (parser.match_expression()) {
      std::cout << parser.ast()->dump();
    }
  }

  std::cout << "\n";

  {
    Compiler::Parser parser("(abc)");
    if (parser.match_expression()) {
      std::cout << parser.ast()->dump();
    }
  }

  std::cout << "\n";

  {
    Compiler::Parser parser("((abc))");
    if (parser.match_expression()) {
      std::cout << parser.ast()->dump();
    }
  }
}
