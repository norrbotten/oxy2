#pragma once

#include "renderer/xsdl/compiler/ast/node.hpp"

namespace Oxy::XSDL::Compiler {

  class DecimalLiteralNode : public ASTNode {
  public:
    DecimalLiteralNode(const std::string& literal)
        : m_literal(literal) {}

    virtual const char* class_name() const override { return "DecimalLiteral"; }
    virtual bool        is_decimal_literal() const override { return true; }

    const auto& literal() const { return m_literal; }

  private:
    std::string m_literal;
  };

} // namespace Oxy::XSDL::Compiler
