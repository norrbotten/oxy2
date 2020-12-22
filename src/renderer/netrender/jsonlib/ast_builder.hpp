#pragma once

#include <vector>

#include "renderer/netrender/jsonlib/ast_node.hpp"

namespace Oxy::NetRender::JSON {

  // TODO: some kind of preallocated pool of nodes to speed up ma dynamic memory

  class ASTBuilder {
  public:
    auto& node() {
      assert(m_stack.size() > 0);
      return m_stack.back();
    }

    void bubble(int offset = 0) {
      assert(m_stack.size() > 1);
      m_stack.at(m_stack.size() - 2 + offset)->push(m_stack.back());
      m_stack.pop_back();
    }

    void swap(int offset = 0) {
      auto n = m_stack.size() - 1;
      std::swap(m_stack.at(n - 1 + offset), m_stack.at(n + offset));
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
