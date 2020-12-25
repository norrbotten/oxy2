#pragma once

#include <optional>

#include "renderer/common/ast_builder.hpp"
#include "renderer/common/parser.hpp"

#include "renderer/xsdl/compiler/ast/node.hpp"

namespace Oxy::XSDL::Compiler {

  using ASTBuilder = Common::ASTBuilder<ASTNode>;

  using Token = std::optional<std::string>;

  class Parser final : public Common::Parser<ASTBuilder> {
  public:
    Parser(const std::string& input)
        : Common::Parser<ASTBuilder>(input) {}

    Token parse_identifier();
    Token parse_integer_literal();

    Token parse_keyword();
  };

} // namespace Oxy::XSDL::Compiler
