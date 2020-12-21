#pragma once

#include <iostream>

#include "renderer/netrender/jsonlib/ast_builder.hpp"

namespace Oxy::NetRender::JSON {

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

    bool match_4_hex_digits() {
      return match([&] {
        for (int i = 0; i < 4; i++) {
          if (!is_hex_digit(ch()))
            return false;

          forward();
        }

        return true;
      });
    }

    bool match_escape_sequence() {
      return match([&] {
        if (ch() == '\\') {
          forward();

          if (ch() == 'u') {
            forward();
            return match_4_hex_digits();
          }
          else if (is_valid_escape(ch())) {
            forward();
            return true;
          }

          return false;
        }
        else
          return false;
      });
    }

    bool match_noncontrol_char() {
      return match([&] {
        if (is_control_char(ch()) || ch() == '"' || ch() == '\\')
          return false;

        forward();

        return true;
      });
    }

    bool match_string_character() {
      return match([&] { return match_escape_sequence() || match_noncontrol_char(); });
    }

    bool match_string() {
      return match([&] {
        if (ch() != '"')
          return false;

        forward();

        while (match_string_character())
          ;

        if (ch() != '"')
          return false;

        forward();

        return true;
      });
    }

    bool match_null() {
      return match([&] {
        if (peek(0, 4) == "null") {
          forward(4);
          return true;
        }

        return false;
      });
    }

    bool match_boolean() {
      return match([&] {
        if (peek(0, 4) == "true") {
          forward(4);
          return true;
        }

        if (peek(0, 5) == "false") {
          forward(5);
          return true;
        }

        return false;
      });
    }

    bool match_object() { return false; }

    bool match_value() {
      return match([&] {
        match_whitespace();

        if (!(match_string() || match_number() || match_object() || match_array() ||
              match_boolean() || match_null())) {
          return false;
        }

        match_whitespace();

        return true;
      });
    }

    bool match_array_separator() {
      return match([&] {
        if (ch() == ',') {
          forward();
          return true;
        }

        return false;
      });
    }

    bool match_array() {
      return match([&] {
        if (ch() != '[')
          return false;

        forward();

        bool trailing_comma = false;

        while (true) {
          if (match_value()) {
            trailing_comma = false;

            if (match_array_separator())
              trailing_comma = true;
          }
          else
            break;
        }

        if (trailing_comma)
          return false;

        match_whitespace();

        if (ch() != ']')
          return false;

        forward();

        return true;
      });
    }

    bool parse() {
      return match([&] { return false; });
    }

  private:
    std::string m_input;
    int         m_position, m_consume_ptr;

    ASTBuilder m_ast;
  }; // namespace Oxy::NetRender::JSON

} // namespace Oxy::NetRender::JSON
