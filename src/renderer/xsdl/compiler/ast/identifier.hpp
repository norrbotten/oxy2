#pragma once

#include "renderer/xsdl/compiler/ast/node.hpp"

namespace Oxy::XSDL::Compiler {

  class IdentifierNode : public ASTNode {
  public:
    IdentifierNode(const std::string& name)
        : m_name(name) {}

    virtual const char* class_name() const override { return "Identifier"; }
    virtual bool        is_identifier() const override { return true; }

    const auto& name() const { return m_name; }

  private:
    std::string m_name;
  };

} // namespace Oxy::XSDL::Compiler
