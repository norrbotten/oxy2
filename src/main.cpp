
#include <iostream>

#include "renderer/primitives/sphere.hpp"

using namespace Oxy;

int main() {
  auto sphere = Primitive::SpherePrimitive(Vec3(0, 0, 0), 1.0);

  auto res = Primitive::Traits::intersect_ray(sphere, SingleRay{Vec3(-5, 1, 0), Vec3(1, 0, 0)});

  if (res.has_value())
    std::cout << res.value() << "\n";

  return 0;
}
