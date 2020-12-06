
#include <iostream>

#include "renderer/camera.hpp"
#include "renderer/sample_film.hpp"

#include "renderer/material/materials.hpp"

#include "renderer/tracing/sphere.hpp"

using namespace Oxy;

int main() {
  auto film = SampleFilm();
  film.resize(1024, 1024);

  auto cam = Camera();
  cam.set_pos(Vec3(-7, 0, 0));
  cam.set_fov(70);
  cam.aim(Vec3(0, 0, 0));

  auto bsdf = std::make_shared<BSDF>(Color(1.0, 0.0, 0.0));

  auto sphere       = TracableSphere(Vec3(0.0, 0.0, 0.0), 1.0);
  sphere.material() = bsdf;

  cam.for_each_pixel(film, [&](SingleRay ray) {
    auto isect = sphere.intersect_ray(ray);

    if (isect.hit) {
      return isect.object->material()->sample(isect);
    }

    return Color(0.0);
  });

  film.write_png("data/out.png");
  return 0;
}
