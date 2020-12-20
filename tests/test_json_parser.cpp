#include "renderer/netrender/jsonlib/parser.hpp"

#include <vector>

#define CASE(input, method, expected_output)                                                       \
  {                                                                                                \
    JSON::Parser parser(input);                                                                    \
    if (parser.method() == expected_output)                                                        \
      std::cout << input << "\t== " << expected_output << " SUCCESS\n";                            \
    else                                                                                           \
      std::cout << input << "\t!= " << expected_output << " FAILED\n";                             \
  }

#define CASE_NUMBER(input, expected_output) CASE(input, match_number, expected_output)

int main() {
  using namespace Oxy::NetRender;

  CASE_NUMBER("0.0", 1);
  CASE_NUMBER("", 0);
  CASE_NUMBER("-5.0", 1);
  CASE_NUMBER(".0", 0);
  CASE_NUMBER("-0", 1);
  CASE_NUMBER("-0.", 0);
  CASE_NUMBER("3.14", 1);
  CASE_NUMBER("314e-2", 1);
  CASE_NUMBER("0.314e2", 1);
}
