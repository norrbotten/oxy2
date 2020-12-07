#include <iostream>

#include <omp.h>

#include "renderer/camera.hpp"
#include "renderer/image.hpp"
#include "renderer/sample_film.hpp"

#include "renderer/integrators/naive.hpp"
#include "renderer/material/materials.hpp"
#include "renderer/tracing/sphere.hpp"

using namespace Oxy;

int main() {
  auto film = SampleFilm();
  film.resize(512, 512);

  auto cam = Camera();
  cam.set_pos(Vec3(-7, 0, 2));
  cam.set_fov(70);
  cam.aim(Vec3(0, 0, 0));

  auto diffuse  = std::shared_ptr<Material>(create_diffuse(Color(1.0, 1.0, 1.0)));
  auto rdiffuse = std::shared_ptr<Material>(create_diffuse(Color(1.0, 0.0, 0.0)));
  auto gdiffuse = std::shared_ptr<Material>(create_diffuse(Color(0.0, 1.0, 0.0)));
  auto bdiffuse = std::shared_ptr<Material>(create_diffuse(Color(0.0, 0.0, 1.0)));

  auto emissive = std::shared_ptr<Material>(create_emissive(Color(1.0, 1.0, 1.0)));

  auto rsphere        = new TracableSphere(Vec3(0.0, 0.0, 0.0), 1.0);
  rsphere->material() = diffuse;

  auto gsphere        = new TracableSphere(Vec3(0.0, 2.0, 0.0), 1.0);
  gsphere->material() = gdiffuse;

  auto bsphere        = new TracableSphere(Vec3(0.0, -2.0, 0.0), 1.0);
  bsphere->material() = rdiffuse;

  auto ground        = new TracableSphere(Vec3(0.0, 0.0, -1e6), 1e6 - 1);
  ground->material() = diffuse;

  auto light        = new TracableSphere(Vec3(0.0, 0.0, 14.5), 10.0);
  light->material() = emissive;

  auto integrator = new Integrators::Naive();

  integrator->world().add_object(rsphere);
  integrator->world().add_object(gsphere);
  integrator->world().add_object(bsphere);

  integrator->world().add_object(ground);
  integrator->world().add_object(light);

  integrator->setup();

  for (int n = 0; n < 512; n++) {
    integrator->pre_pass();
    cam.for_each_pixel(film, [&](SingleRay ray) { return integrator->radiance(ray); });
    integrator->post_pass();

    std::cout << n << "\n";
  }

  delete integrator; // shaddap clang-tidy

  auto image = Image(film);
  image.extended_reinhard().gamma(2.2).clamp().write_png("images/out2.png");

  return 0;
}
