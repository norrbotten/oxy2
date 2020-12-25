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
}
