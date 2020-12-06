
#include <iostream>

#include "renderer/material/materials.hpp"

using namespace Oxy;

int main() {

  IntersectionContext ctx;

  ctx.ray.origin    = Vec3(-5, 0, 0);
  ctx.ray.direction = Vec3(1, 0, 0);

  ctx.hitnormal = Vec3(-1, 0, 0);
  ctx.hitpos    = Vec3(-1, 0, 0);

  auto* material = create_glossy(Color(1.0, 1.0, 1.0), 1);

  auto res = material->scatter(ctx);

  std::cout << res.ray.direction << "\n";

  return 0;
}
