
#include <iostream>

#include "renderer/tracing/utils.hpp"

using namespace Oxy;

int main() {

  for (int i = 0; i < 100; i++) {
    std::cout << random_vector_on_cone(Vec3{0, 0, 1}, M_PI / 16) << "\n";
  }

  return 0;
}
