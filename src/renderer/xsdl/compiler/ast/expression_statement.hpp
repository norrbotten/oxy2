#pragma once

#include "renderer/xsdl/compiler/ast/statement.hpp"

namespace Oxy::XSDL::Compiler {

  class ExpressionStatementNode : public StatementNode {
  public:
    virtual const char* class_name() const override { return "ExpressionStatementNode"; }

    virtual bool is_expression_statement() const override { return true; }
  };

} // namespace Oxy::XSDL::Compiler
