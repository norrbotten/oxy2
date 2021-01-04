
#include <iostream>

#include "renderer/xsdl/compiler/ast.hpp"
#include "renderer/xsdl/compiler/tokenizer.hpp"

int main() {
  using namespace Oxy::XSDL::Compiler;

  {
    Tokenizer tokenizer("{;;;;}");
    tokenizer.process();

    for (auto& token : tokenizer.ctokens())
      std::cout << token.stringify() << "\n";

    ASTBuilder ast_builder(tokenizer.ctokens());

    if (ast_builder.process()) {
      auto node = ast_builder.ast();
      std::cout << node->class_name() << "\n";
    }
  }
}
