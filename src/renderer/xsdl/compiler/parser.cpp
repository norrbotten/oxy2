#include "renderer/xsdl/compiler/parser.hpp"

namespace Oxy::XSDL::Compiler {

  // Parses an identifier, basically a string of valid alphanumeric characters including _$ but not
  // starting with a number
  Token Parser::parse_identifier() {
    if (match([&] {
          skip_whitespace();

          if (!Common::is_valid_first_identifier_char(ch()))
            return false;

          discard();

          while (Common::is_valid_identifier_char(ch()))
            forward();

          skip_whitespace();

          return true;
        })) {
      return consume();
    }

    return {};
  }

  // Parses an integer literal
  Token Parser::parse_integer_literal() {
    if (match([&] {
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

  // Parses any keyword
  Token Parser::parse_keyword() {
    if (match([&] { return true; })) {
      return consume();
    }

    return {};
  }

} // namespace Oxy::XSDL::Compiler
