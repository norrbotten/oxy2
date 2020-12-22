#include "renderer/netrender/jsonlib/json.hpp"

int main() {
  using namespace Oxy::NetRender::JSON;

  {
    JSONParser parser("[1, 2, 3, 4, 5]");

    if (parser.success()) {
      auto value = parser.json();
      if (value->is_array()) {
        auto arr = ((JSONArray*)value)->to_vector<double>();

        for (auto& val : arr)
          std::cout << val << "\n";
      }
    }
  }

  std::cout << "\n";

  {
    JSONParser parser("[true, false, true, false, false]");

    if (parser.success()) {
      auto value = parser.json();
      if (value->is_array()) {
        auto arr = ((JSONArray*)value)->to_vector<bool>();

        for (auto val : arr)
          std::cout << val << "\n";
      }
    }
  }

  std::cout << "\n";

  {
    JSONParser parser(R"""(["a", "b", "c", "d", "e"])""");
    if (parser.success()) {
      auto value = parser.json();
      if (value->is_array()) {
        auto arr = ((JSONArray*)value)->to_vector<std::string>();

        for (auto& val : arr)
          std::cout << val << "\n";
      }
    }
  }
}
