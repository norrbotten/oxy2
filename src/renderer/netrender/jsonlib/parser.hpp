#pragma once

#include <iostream>

namespace Oxy::NetRender::JSON {

  inline bool is_digit_1_to_9(char ch) { return ch >= '1' && ch <= '9'; }
  inline bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }

  class Parser {
  public:
    Parser(const std::string& input)
        : m_input(input)
        , m_position(0)
        , m_consume_ptr(0) {}

  private:
    void forward(unsigned int step = 1) { m_position += step; }

    bool eof(unsigned int offset = 0) { return (m_position + offset) >= m_input.size(); }

    char ch(unsigned int offset = 0) {
      if (eof(offset))
        return '\0';

      return m_input.at(m_position + offset);
    }

    std::string peek(unsigned int offset = 0, unsigned int length = 1) {
      if (eof(offset + length))
        return "";

      return m_input.substr(m_position + offset, length);
    }

    std::string consume() {
      if (m_position == m_consume_ptr)
        return "";

      auto str      = m_input.substr(m_consume_ptr, m_position - m_consume_ptr);
      m_consume_ptr = m_position;

      return str;
    }

    template <typename MatcherFunctor>
    bool match(MatcherFunctor matcher) {
      int pos = m_position;

      auto res = matcher();
      if (!res)
        m_position = pos;

      return res;
    }

    template <typename MatcherFunctor>
    bool optional(MatcherFunctor matcher) {
      return matcher();
    }

    bool match_whitespace() {
      return match([&] {
        auto moved = false;

        while (ch() == ' ' || ch() == '\n' || ch() == '\r' || ch() == '\t') {
          forward();
          moved = true;
        }

        return moved;
      });
    }

  public:
    bool match_number_sign() {
      return match([&] {
        if (ch() == '-')
          forward();

        return true;
      });
    }

    bool match_number_fraction() {
      return match([&] {
        if (ch() == '.') {
          forward();

          if (!is_digit(ch()))
            return false;

          while (is_digit(ch()))
            forward();

          return true;
        }
        else
          return true;
      });
    }

    bool match_number_exponent() {
      return match([&] {
        if (ch() == 'e' || ch() == 'E') {
          forward();

          if (ch() == '-' || ch() == '+')
            forward();

          if (!is_digit(ch()))
            return false;

          forward();

          while (is_digit(ch()))
            forward();

          return true;
        }
        else
          return true;
      });
    }

    bool match_number_pre_fraction() {
      return match([&] {
        if (ch() == '0') {
          forward();
          return true;
        }
        else {
          if (!is_digit_1_to_9(ch()))
            return false;

          forward();

          while (is_digit(ch()))
            forward();

          return true;
        }
      });
    }

    bool match_number() {
      return match([&] {
        optional([&] { return match_number_sign(); });

        if (!match_number_pre_fraction())
          return false;

        return match_number_fraction() && match_number_exponent();
      });
    }

  private:
    std::string m_input;

    int m_position, m_consume_ptr;
  }; // namespace Oxy::NetRender::JSON

} // namespace Oxy::NetRender::JSON
