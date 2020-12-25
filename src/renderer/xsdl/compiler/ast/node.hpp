#pragma once

#include <string>
#include <vector>

namespace Oxy::XSDL::Compiler {

  class ASTNode {
  public:
    virtual ~ASTNode() {
      for (auto& child : m_children)
        delete child;
    }

    void push(ASTNode* node) { m_children.push_back(node); }

    virtual const char* class_name() const { return "***ASTNode***"; }
    ASTNode*            upcast() const { return (ASTNode*)this; };

    virtual bool is_identifier() const { return false; }
    virtual bool is_statement() const { return false; }
    virtual bool is_expression_statement() const { return false; }

  private:
    std::vector<ASTNode*> m_children;
  };

} // namespace Oxy::XSDL::Compiler
