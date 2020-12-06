
#include <iostream>

#include "renderer/camera.hpp"
#include "renderer/sample_film.hpp"

#include "renderer/material/materials.hpp"

#include "renderer/primitives/sphere.hpp"

using namespace Oxy;

int main() {
  auto film = SampleFilm();
  film.resize(1024, 1024);

  auto cam = Camera();
  cam.set_pos(Vec3(-5, 0, 0));
  cam.set_fov(70);
  cam.aim(Vec3(0, 0, 0));

  auto sphere  = Primitive::SpherePrimitive(Vec3(0, 0, 0), 1.0);
  namespace TR = Primitive::Traits;

  cam.for_each_pixel(film, [&](SingleRay ray) {
    if (auto isect = TR::intersect_ray(sphere, ray); isect.has_value()) {
      IntersectionContext ctx;
      ctx.ray = ray;
      ctx.hit = true;

      ctx.t         = isect.value();
      ctx.hitpos    = ray.origin + ray.direction * isect.value();
      ctx.hitnormal = TR::hitnormal(sphere, ctx.hitpos);

      ctx.uv = {0.0, 0.0};

      return Color(glm::dot(ray.direction, -ctx.hitnormal));
    }

    return Color(0.0);
  });

  film.write_png("data/out.png");
  return 0;
}
