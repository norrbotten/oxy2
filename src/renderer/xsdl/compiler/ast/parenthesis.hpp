#pragma once

#include "renderer/xsdl/compiler/ast/node.hpp"

namespace Oxy::XSDL::Compiler {

  enum class ParenthesisType {
    Left,
    Right,
  };

  class ParenthesisNode : public ASTNode {
  public:
    ParenthesisNode(ParenthesisType type)
        : m_type(type) {}

    virtual const char* class_name() const override { return "Parenthesis"; }
    virtual bool        is_parenthesis() const override { return true; }

    bool is_left() const { return m_type == ParenthesisType::Left; }
    bool is_right() const { return m_type == ParenthesisType::Right; }

    void push(ASTNode*) = delete;

  private:
    ParenthesisType m_type;
  };

} // namespace Oxy::XSDL::Compiler
