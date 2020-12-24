#pragma once

#include <string>

#include "renderer/common/ast_builder.hpp"

namespace Oxy::Common {

  inline bool is_digit_1_to_9(char ch) { return ch >= '1' && ch <= '9'; }
  inline bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }

  inline bool is_hex_digit(char ch) {
    return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
  }

  inline bool is_control_char(char ch) { return ch < 32; }

  inline bool is_valid_escape(char ch) {
    switch (ch) {
    case '\\':
    case '"':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
    case 'u': return true;
    default: return false;
    }
  }

  template <typename TASTBuilder>
  class Parser {
  public:
    Parser(const std::string& input)
        : m_input(input)
        , m_position(0)
        , m_consume_ptr(0) {}

  protected:
    void forward(unsigned int step = 1) { m_position += step; }

    bool eof(unsigned int offset = 0) { return (m_position + offset) >= m_input.size(); }

    char ch(unsigned int offset = 0) {
      if (eof(offset))
        return '\0';

      return m_input.at(m_position + offset);
    }

    std::string peek(unsigned int offset = 0, unsigned int length = 1) {
      return m_input.substr(m_position + offset, length);
    }

    void discard() { m_consume_ptr = m_position; }

    std::string consume() {
      if (m_position == m_consume_ptr)
        return "";

      auto str      = m_input.substr(m_consume_ptr, m_position - m_consume_ptr);
      m_consume_ptr = m_position;

      return str;
    }

    template <typename MatcherFunctor>
    bool match(MatcherFunctor matcher) {
      int pos  = m_position;
      int cpos = m_consume_ptr;

      auto res = matcher();
      if (!res) {
        m_position    = pos;
        m_consume_ptr = cpos;
      }

      return res;
    }

    template <typename MatcherFunctor>
    bool optional(MatcherFunctor matcher) {
      return matcher();
    }

    void skip_whitespace() {
      while (ch() == ' ' || ch() == '\n' || ch() == '\r' || ch() == '\t') {
        forward();
      }
    }

    auto ast() { return m_ast.ast(); }

  protected:
    std::string m_input;
    int         m_position, m_consume_ptr;

    TASTBuilder m_ast;
  };

} // namespace Oxy::Common
