#pragma once

#include "renderer/common/ast_builder.hpp"
#include "renderer/common/parser.hpp"

#include "renderer/xsdl/compiler/ast/node.hpp"

namespace Oxy::XSDL::Compiler {

  using ASTBuilder = Common::ASTBuilder<ASTNode>;

  class Parser final : public Common::Parser<ASTBuilder> {
  public:
    Parser(const std::string& input)
        : Common::Parser<ASTBuilder>(input) {}
  };

} // namespace Oxy::XSDL::Compiler
