#pragma once

#include <vector>

#include "renderer/netrender/jsonlib/ast_node.hpp"

namespace Oxy::NetRender::JSON {

  class ASTBuilder {
  public:
    ASTBuilder()
        : m_root(nullptr) {}

    void set_root(ASTNode* root) {
      m_root = root;
      select(m_root);
    }

    auto at() const { return m_stack.back(); }

    void select(ASTNode* node) { m_stack.push_back(node); }
    void back() { m_stack.pop_back(); }

    auto ast() { return m_root; }

  private:
    ASTNode* m_root;

    std::vector<ASTNode*> m_stack;
  };

} // namespace Oxy::NetRender::JSON
