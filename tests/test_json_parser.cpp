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

#define CASE_STRING(input, expected_output) CASE(input, match_string, expected_output)

#define CASE_ARRAY(input, expected_output) CASE(input, match_array, expected_output)

#define CASE_OBJECT(input, expected_output) CASE(input, match_object, expected_output)

#define CASE_VALUE(input, expected_output) CASE(input, match_value, expected_output)

int main() {
  using namespace Oxy::NetRender;

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

  CASE_STRING("\"abc\"", 1);
  CASE_STRING("\"abc", 0);
  CASE_STRING("abc\"", 0);
  CASE_STRING("\"a\\nc\"", 1);
  CASE_STRING("\"\\u0000\"", 1);
  CASE_STRING("\"\\u00\"  ", 0);

  CASE_ARRAY("[]", 1);
  CASE_ARRAY("[1]", 1);
  CASE_ARRAY("[1, 2]", 1);
  CASE_ARRAY("[1,2 ,]", 0);
  CASE_ARRAY("[1,2,[1,2],[1,2,[1,2,[1,2]]]]", 1);
}
