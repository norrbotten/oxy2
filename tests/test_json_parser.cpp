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

#define CASE_BOOLEAN(input, expected_output) CASE(input, match_boolean, expected_output)

#define CASE_NUMBER(input, expected_output) CASE(input, match_number, expected_output)

#define CASE_STRING(input, expected_output) CASE(input, match_string, expected_output)

#define CASE_ARRAY(input, expected_output) CASE(input, match_array, expected_output)

#define CASE_OBJECT(input, expected_output) CASE(input, match_object, expected_output)

#define CASE_VALUE(input, expected_output) CASE(input, match_value, expected_output)

int main() {
  using namespace Oxy::NetRender;

  std::cout << "booleans\n";

  CASE_BOOLEAN("true", 1);
  CASE_BOOLEAN("false", 1);
  CASE_BOOLEAN("tru", 0);
  CASE_BOOLEAN("fals", 0);
  CASE_BOOLEAN("fasle", 0);
  CASE_BOOLEAN("null", 0);

  std::cout << "\nnumbers\n";

  CASE_NUMBER("1", 1);
  CASE_NUMBER("0", 1);
  CASE_NUMBER("59", 1);
  CASE_NUMBER("0.0", 1);
  CASE_NUMBER("", 0);
  CASE_NUMBER("-5.0", 1);
  CASE_NUMBER(".0", 0);
  CASE_NUMBER("-0", 1);
  CASE_NUMBER("-0.", 0);
  CASE_NUMBER("3.14", 1);
  CASE_NUMBER("314e-2", 1);
  CASE_NUMBER("0.314e2", 1);

  std::cout << "\nstrings\n";

  CASE_STRING("\"abc\"", 1);
  CASE_STRING("\"abc", 0);
  CASE_STRING("abc\"", 0);
  CASE_STRING("\"a\\nc\"", 1);
  CASE_STRING("\"\\u0000\"", 1);
  CASE_STRING("\"\\u00\"  ", 0);

  std::cout << "\narrays\n";

  CASE_ARRAY("[]", 1);
  CASE_ARRAY("[1]", 1);
  CASE_ARRAY("[1, 2]", 1);
  CASE_ARRAY("[1,2 ,]", 0);
  CASE_ARRAY("[1,2,[1,2],[1,2,[1,2,[1,2]]]]", 1);

  std::cout << "\nobjects\n";

  CASE_OBJECT(R"""({})""", 1);
  CASE_OBJECT(R"""({"a": null})""", 1);
  CASE_OBJECT(R"""({"a": {}}})""", 1);
  CASE_OBJECT(R"""({"a": []})""", 1);
  CASE_OBJECT(R"""({"a" []})""", 0);
  CASE_OBJECT(R"""({"a": null,})""", 0);
  CASE_OBJECT(R"""({"a": {},})""", 0);
  CASE_OBJECT(R"""({"a": [],})""", 0);
  CASE_OBJECT(R"""({"a" [],})""", 0);

  CASE_OBJECT(R"""({"a": null, "b": 10})""", 1);
  CASE_OBJECT(R"""({"a": {}, "b": [1, [1, 2]]})""", 1);
  CASE_OBJECT(R"""({"a": [], "b": null})""", 1);

  CASE_OBJECT(R"""({"a" [], "b": false})""", 0);
  CASE_OBJECT(R"""({"a": [], "b" false})""", 0);
}
