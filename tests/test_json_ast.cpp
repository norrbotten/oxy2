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

  CASE_JSON(R"""([1, 2, 3, 4, 5, 6, 7, "8"])""", 1);
  CASE_JSON(R"""([1, 2, 3, [4, -4], 5, 6, 7, "8"])""", 1);

  {
    JSON::Parser parser(R"""([1, 2, [10, 11], {"a": 3, "b": {"a": [1, 2, 3]}}, 5, 6])""");
    if (parser.parse()) {
      std::cout << parser.ast()->dump() << "\n";

      auto json = parser.ast()->to_json_value();
      std::cout << json << "\n";

      delete json;
    }
  }

  /*
  {
    JSON::Parser parser("[1, 2, 3, {}, 5, 6]");
    if (parser.parse()) {
      std::cout << parser.ast()->stringify() << "\n";
    }
  }
  */
}
