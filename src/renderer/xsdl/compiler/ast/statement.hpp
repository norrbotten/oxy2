#pragma once

#include "renderer/xsdl/compiler/ast/node.hpp"

namespace Oxy::XSDL::Compiler {

  class StatementNode : public ASTNode {
  public:
    virtual const char* class_name() const override { return "StatementNode"; }

    virtual bool is_statement() const override { return true; }
  };

} // namespace Oxy::XSDL::Compiler
