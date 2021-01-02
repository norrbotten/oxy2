#pragma once

#include <vector>

#include "renderer/xsdl/compiler/token.hpp"

namespace Oxy::XSDL::Compiler {

  class ASTBuilder {
  public:
    ASTBuilder(const std::vector<Token>& tokens)
        : m_tokens_ref(tokens) {}

  private:
    const std::vector<Token>& m_tokens_ref;
  };

} // namespace Oxy::XSDL::Compiler
