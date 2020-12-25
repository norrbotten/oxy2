#pragma once

#include "renderer/xsdl/compiler/ast/statement.hpp"

namespace Oxy::XSDL::Compiler {

  class ExpressionStatementNode : public ASTNode {
  public:
    virtual const char* class_name() const override { return "ExpressionStatement"; }
    virtual bool        is_expression_statement() const override { return true; }
  };

} // namespace Oxy::XSDL::Compiler
