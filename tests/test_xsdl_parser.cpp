#include "renderer/xsdl/xsdl.hpp"

#include <iostream>

#define CASE(name, method, input)                                                                  \
  {                                                                                                \
    std::cout << name << " - ";                                                                    \
    Compiler::Parser parser(input);                                                                \
    auto             ident = parser.method();                                                      \
    if (ident.has_value())                                                                         \
      std::cout << ident.value() << "\n";                                                          \
    else                                                                                           \
      std::cout << "***Did not match***\n";                                                        \
  }

int main() {
  using namespace Oxy::XSDL;

  CASE("parse identifier", parse_identifier, "_abc12$hello");
  CASE("parse identifier", parse_identifier, "____");

  CASE("parse integer literal", parse_integer_literal, "123");
  CASE("parse integer literal", parse_integer_literal, "-123");
  CASE("parse integer literal", parse_integer_literal, "-5");

  CASE("parse keyword", parse_keyword, "if");
  CASE("parse keyword", parse_keyword, "else");
  CASE("parse keyword", parse_keyword, "else if");
  CASE("parse keyword", parse_keyword, "while");
  CASE("parse keyword", parse_keyword, "for");
  CASE("parse keyword", parse_keyword, "return");

  CASE("parse decimal literal", parse_decimal_literal, "1.0");
  CASE("parse decimal literal", parse_decimal_literal, "1.0e4");
  CASE("parse decimal literal", parse_decimal_literal, "3e4");
  CASE("parse decimal literal", parse_decimal_literal, "0.5e2");
  CASE("parse decimal literal", parse_decimal_literal, "10.0");

  CASE("parse operator", parse_operator, "+");
  CASE("parse operator", parse_operator, "++");
}
