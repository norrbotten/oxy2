#include <iostream>

#include <omp.h>

#include "renderer/camera.hpp"
#include "renderer/image.hpp"
#include "renderer/sample_film.hpp"

#include "renderer/integrators/naive.hpp"
#include "renderer/material/materials.hpp"
#include "renderer/tracing/sdf.hpp"
#include "renderer/tracing/sphere.hpp"

using namespace Oxy;

int main() {
  auto film = SampleFilm();
  film.resize(1024, 1025);

  auto cam = Camera();
  cam.set_pos(Vec3(-9, 9, 9));
  cam.set_fov(70);
  cam.aim(Vec3(0, 0, -1));

  // auto diffuse  = std::shared_ptr<Material>(create_diffuse(Color(1.0)));
  auto glossy   = std::shared_ptr<Material>(create_glossy(Color(1.0), 0.8));
  auto glossy2  = std::shared_ptr<Material>(create_glossy(Color(0.8, 1.0, 0.8), 0.4));
  auto emissive = std::shared_ptr<Material>(create_emissive(Color(10.0, 8.0, 8.0)));

  auto ground        = new TracableSphere(Vec3(0.0, 0.0, -1e6), 1e6 - 1);
  ground->material() = glossy2;

  auto light        = new TracableSphere(Vec3(0.0, 0.0, 2.0), 2.0);
  light->material() = emissive;

  auto sdf = new TracableSDF(Vec3(0.0), 1.0, [](const Vec3& point) {
    return SDFPos::repeat_finite(point, Vec3(1.5), Vec3(3), SDFShape::sphere, 0.7);
  });

  sdf->material() = glossy;

  auto integrator = new Integrators::Naive();

  integrator->world().add_object(sdf);
  integrator->world().add_object(ground);
  integrator->world().add_object(light);

  for (int n = 0; n < 40000; n++) {
    cam.for_each_pixel(film, [&](SingleRay ray) { return integrator->radiance(ray); });
    std::cout << n << "\n";
  }

  delete integrator; // shaddap clang-tidy

  auto image = Image(film);
  image.extended_reinhard().gamma(2.2).clamp().write_png("images/spheres_20000.png");

  return 0;
}
