#include "renderer/parsers/obj.hpp"

int main() {
  using namespace Oxy;

  auto obj = Parsers::OBJ::parse_obj("../data/blueyellow.obj");

  for (auto& a : obj) {
    std::cout << "Mesh with " << a.facets.size() << " material(s)\n";
    for (auto& b : a.facets) {
      std::cout << "    " << b.second.size() << " triangles\n";
    }
  }
}