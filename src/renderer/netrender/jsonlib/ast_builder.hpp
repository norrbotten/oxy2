#pragma once

#include <vector>

#include "renderer/netrender/jsonlib/ast_node.hpp"

namespace Oxy::NetRender::JSON {

  class ASTBuilder {
  public:
    ASTBuilder() {}

    auto& node() {
      assert(m_stack.size() > 0);
      return m_stack.back();
    }

    void bubble() {
      assert(m_stack.size() > 1);
      m_stack.at(m_stack.size() - 2)->push(m_stack.back());
      m_stack.pop_back();
    }

    void push(ASTNode* node) { m_stack.push_back(node); }

    void reject() {
      auto node = m_stack.back();
      m_stack.pop_back();
      delete node;
    }

    auto ast() { return m_stack.size() > 0 ? m_stack.back() : nullptr; }

  private:
    std::vector<ASTNode*> m_stack;
  };

} // namespace Oxy::NetRender::JSON
