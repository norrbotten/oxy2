#pragma once

#include <string>
#include <vector>

namespace Oxy::XSDL::Compiler {

  class ASTNode {
  public:
    ~ASTNode() {
      for (auto& child : m_children)
        delete child;
    }

    void push(ASTNode* node) { m_children.push_back(node); }

  private:
    std::vector<ASTNode*> m_children;
  };

} // namespace Oxy::XSDL::Compiler
