#pragma once

#include <iostream>
#include <string>
#include <utility>

namespace Oxy::XSDL::Compiler {

  class Streamer {
  public:
    Streamer(const std::string& input)
        : m_input(input) {}

    void setpos(int pos, int consume) {
      m_position = pos;
      m_consume  = consume;
    }

    std::pair<int, int> getpos() const { return {m_position, m_consume}; }

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

    std::pair<std::string, int> consume() {
      if (m_position == m_consume)
        return {"", 0};

      m_last_consume_pos = m_position;

      auto str  = m_input.substr(m_consume, m_position - m_consume);
      m_consume = m_position;

      return {str, m_position - m_last_consume_pos};
    }

    std::string get_parsed() const {
      if (m_position == m_consume)
        return "";

      return m_input.substr(m_consume, m_position - m_consume);
    }

    void discard() { m_consume = m_position; }

  private:
    std::string m_input;
    int         m_position{0}, m_consume{0}, m_last_consume_pos{0};
  };

} // namespace Oxy::XSDL::Compiler
