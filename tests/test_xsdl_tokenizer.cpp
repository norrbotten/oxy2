#include <iostream>

#include "renderer/xsdl/compiler/tokenizer.hpp"

#include "renderer/utils.hpp"

int main() {
  using namespace Oxy::XSDL::Compiler;

  /*
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

  std::cout << "\n";

  {
    try {
      Tokenizer tokenizer("if elif else while ;\n&& || + ++");
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
      Tokenizer tokenizer("true true false false");
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
      Tokenizer tokenizer("while true { break; }");
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
      Tokenizer tokenizer("let const a = 10;");
      tokenizer.process();

      for (auto& token : tokenizer.ctokens())
        std::cout << token.stringify() << "\n";
    }
    catch (const TokenizationError& e) {
      std::cout << "Exception: " << e.what() << "\n";
    }
  }

  std::cout << "\n";

  */

  {
    auto content = Oxy::load_file_into_string("../data/example1.xsdl");
    if (content) {
      try {
        Tokenizer tokenizer(content.value());
        tokenizer.process();

        for (auto& token : tokenizer.ctokens())
          std::cout << token.stringify() << "\n";
      }
      catch (const TokenizationError& e) {
        std::cout << "Exception: " << e.what() << "\n";
      }
    }
  }
}
