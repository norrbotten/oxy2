#pragma once

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Oxy::XSDL::Compiler {

  class ASTNode {
  public:
    virtual ~ASTNode() {
      for (auto& child : m_children)
        delete child;
    }

    auto& parent() { return m_parent; }
    auto& children() { return m_children; }

    void push(ASTNode* node) {
      m_children.push_back(node);
      node->parent() = this;
    }

    void hoist() {
      // need a grandparent to hoist
      assert(parent() != nullptr);
      assert(parent()->parent() != nullptr);

      // remove self from parent
      auto& pchildren = parent()->children();
      pchildren.erase(std::find(pchildren.begin(), pchildren.end(), this));

      // add to parents parent
      parent()->parent()->push(this);
    }

    virtual const char* class_name() const { return "***ASTNode***"; }
    ASTNode*            upcast() const { return (ASTNode*)this; };

    std::string dump(int indent = 0) const {
      std::stringstream ss;

      auto getindent = [](int n) {
        std::stringstream ss;
        for (int i = 0; i < n; i++)
          ss << "  ";
        return ss.str();
      };

      ss << getindent(indent);
      ss << class_name() << "\n";

      for (auto& child : m_children)
        ss << child->dump(indent + 1);

      return ss.str();
    }

    virtual bool is_statement() const { return false; }
    virtual bool is_expression_statement() const { return false; }
    virtual bool is_parenthesis() const { return false; }
    virtual bool is_identifier() const { return false; }
    virtual bool is_integer_literal() const { return false; }
    virtual bool is_decimal_literal() const { return false; }

  private:
    std::vector<ASTNode*> m_children;
    ASTNode*              m_parent = nullptr;
  };

} // namespace Oxy::XSDL::Compiler
