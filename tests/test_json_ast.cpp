#include "renderer/netrender/jsonlib/parser.hpp"

#include <vector>

#define CASE_JSON(input, expected_output)                                                          \
  {                                                                                                \
    JSON::Parser parser(input);                                                                    \
    if (parser.parse() == expected_output)                                                         \
      std::cout << input << "\t== " << expected_output << " SUCCESS\n";                            \
    else                                                                                           \
      std::cout << input << "\t!= " << expected_output << " FAILED\n";                             \
  }

int main() {
  using namespace Oxy::NetRender;

  CASE_JSON(R"""({})""", 1);
  CASE_JSON(R"""({"a": 10})""", 1);
  CASE_JSON(R"""({"a": {"a": {"a": {"a": {"a": {"a": {"a": {"a": {"a": {"a": 10}}}}}}}}}})""", 1);
}
