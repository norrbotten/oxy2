#include "renderer/xsdl/compiler/parser.hpp"

#include <array>
#include <cstring>

namespace Oxy::XSDL::Compiler {

  // Parses an identifier, basically a string of valid alphanumeric characters including _$ but not
  // starting with a number
  Token Parser::parse_identifier() {
    if (match([&] {
          if (!Common::is_valid_first_identifier_char(ch()))
            return false;

          discard();

          while (Common::is_valid_identifier_char(ch()))
            forward();

          return true;
        })) {
      return consume();
    }

    return {};
  }

  // Parses an integer literal
  Token Parser::parse_integer_literal() {
    if (match([&] {
          discard();

          if (ch() == '-')
            forward();

          if (!Common::is_digit(ch()))
            return false;

          while (Common::is_digit(ch()))
            forward();

          return true;
        })) {
      return consume();
    }

    return {};
  }

  // Parses a decimal number literal
  Token Parser::parse_decimal_literal() {
    if (match([&] {
          discard();

          // sign
          if (ch() == '-')
            forward();

          // digits before .
          while (Common::is_digit(ch()))
            forward();

          if (ch() == '.') {
            forward();

            // need at least 1 digit after .
            if (!Common::is_digit(ch()))
              return false;

            while (Common::is_digit(ch()))
              forward();
          }

          // might have an exponent
          if (ch() == 'e' || ch() == 'E') {
            forward();

            // exponent sign
            if (ch() == '-' || ch() == '+')
              forward();

            // need a digit after exponent
            if (!Common::is_digit(ch()))
              return false;

            while (Common::is_digit(ch()))
              forward();
          }

          return true;
        })) {
      return consume();
    }

    return {};
  }

  // Parses any keyword
  Token Parser::parse_keyword() {
    static auto keywords = std::array{
        "if", "else", "else if", "for", "while", "return",
    };

    if (match([&] {
          for (auto& word : keywords) {
            auto len = strlen(word);
            if (peek(0, len) == word) {
              forward(len);
              return true;
            }
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

  // Parses any operator
  Token Parser::parse_operator() {
    /* clang-format off */
    // All operators, these need to be in ordered longest first
    static auto operators = std::array{
      "&&",
      "||",
      "+=",
      "-=",
      "*=",
      "/=",
      "==",
      "!=",
      "++",
      "--",
      "+",
      "-",
      "*",
      "/",
      ",",
      "=",
    };
    /* clang-format on */

    if (match([&] {
          for (auto& op : operators) {
            auto len = strlen(op);
            if (peek(0, len) == op) {
              forward(len);
              return true;
            }
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

} // namespace Oxy::XSDL::Compiler
