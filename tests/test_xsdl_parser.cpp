#include <iostream>

#include "renderer/xsdl/compiler/tokenizer.hpp"

int main() {
  using namespace Oxy::XSDL::Compiler;

  {
    try {
      Tokenizer tokenizer("1000 3000 \"a\\nbc\"");
      tokenizer.process();

      for (auto& token : tokenizer.ctokens())
        std::cout << token.stringify() << "\n";
    }
    catch (const TokenizationError& e) {
      std::cout << "Exception: " << e.what() << "\n";
    }
  }

  std::cout << "\n";

  {
    try {
      Tokenizer tokenizer("true false true 1 2 3\n2 3 5");
      tokenizer.process();

      for (auto& token : tokenizer.ctokens())
        std::cout << token.stringify() << "\n";
    }
    catch (const TokenizationError& e) {
      std::cout << "Exception: " << e.what() << "\n";
    }
  }
}
