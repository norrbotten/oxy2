#pragma once

#include <cassert>
#include <vector>

namespace Oxy::Common {

  // TODO: some kind of preallocated pool of nodes to speed up ma dynamic memory

  template <typename T>
  class ASTBuilder {
  public:
    ~ASTBuilder() {
      if (m_stack.size() == 0)
        return;

      // if theres more than 1 child, even in case of parsing failure, something went wrong
      // because nodes should have been rejected
      // this may change though if i decide to use throw/catch for error handling
      assert(m_stack.size() == 1);

      delete m_stack.front();
    }

    auto& node() {
      assert(m_stack.size() > 0);
      return m_stack.back();
    }

    void bubble(int offset = 0) {
      assert(m_stack.size() > 1);
      m_stack.at(m_stack.size() - 2 + offset)->push(m_stack.back());
      m_stack.pop_back();
    }

    auto last_pushed_node() const { return m_last_pushed_node; }

    void reverse(int count, int offset = 0) {
      assert(m_stack.size() - count - offset > 0);
      std::reverse(m_stack.end() - count - offset, m_stack.end() - offset);
    }

    void push(T* node) {
      m_stack.push_back(node);
      m_last_pushed_node = node;
    }

    void reject() {
      auto node = m_stack.back();
      m_stack.pop_back();
      delete node;
    }

    auto ast() { return m_stack.size() > 0 ? m_stack.back() : nullptr; }

  private:
    std::vector<T*> m_stack;
    T*              m_last_pushed_node = nullptr;
  };

} // namespace Oxy::Common
