#include <iomanip>
#include <iostream>
#include <string>

#include <omp.h>
#include <signal.h>

#include "renderer/timer.hpp"

#include "renderer/camera.hpp"
#include "renderer/image.hpp"
#include "renderer/sample_film.hpp"

#include "renderer/integrators/naive.hpp"
#include "renderer/material/materials.hpp"
#include "renderer/tracing/sdf.hpp"
#include "renderer/tracing/sphere.hpp"

using namespace Oxy;

static bool break_loop = false;

int main() {

  signal(
      SIGINT, *[](int) {
        std::cout << "breaking loop..\n";
        break_loop = true;
      });

  auto film = SampleFilm();
  film.resize(1024, 1025);

  auto cam = Camera();
<<<<<<< HEAD
  cam.set_pos(Vec3(-6, 6, 13));
=======
  cam.set_pos(Vec3(-8, 8, 13));
>>>>>>> 6224e88dd7e700d2566b5022d276a8f33b94a743
  cam.set_fov(70);
  cam.aim(Vec3(0, 0, -1));

  auto diffuse = std::shared_ptr<Material>(create_diffuse(Color(0.0, 0.0, 1.0)));
  auto glossy  = std::shared_ptr<Material>(create_glossy(Color(1.0), 0.8));
  auto glossy2 = std::shared_ptr<Material>(create_glossy(Color(1.0, 1.0, 1.0), 0.5));

  auto emissive      = std::shared_ptr<Material>(create_emissive(Color(30.0)));
  auto blue_emissive = std::shared_ptr<Material>(create_emissive(Color(0.0, 0.0, 150.0)));

  auto ground        = new TracableSphere(Vec3(0.0, 0.0, -1e6), 1e6 - 1);
  ground->material() = glossy2;

  auto light        = new TracableSphere(Vec3(0.0, 0.0, 12.0), 1.0);
  light->material() = emissive;

  auto sdf = new TracableSDF(Vec3(0.0), 1.0, [](const Vec3& point) {
    auto spheres = SDFPos::repeat_finite(point, Vec3(1.5), Vec3(3), SDFShape::sphere, 0.7);
    auto cut     = SDFShape::box(point, Vec3(3.8, 3.8, 8));
    return SDFOp::sub(cut, spheres);
  });

  sdf->material() = glossy;

  auto torus_sdf = new TracableSDF(Vec3(0.0), 1.0, [](const Vec3& point) {
    return SDFShape::capsule(point, Vec3(0, 0, 4), Vec3(0, 0, -1), 0.15);
  });

  torus_sdf->material() = blue_emissive;

  auto integrator = new Integrators::Naive();

  integrator->world().add_object(sdf);
  integrator->world().add_object(torus_sdf);
  integrator->world().add_object(ground);
  integrator->world().add_object(light);

  Timer timer;

  int done   = 0;
  int target = next_sample_target(done);

  timer.start();

  while (true) {
    cam.for_each_pixel(film, [&](SingleRay ray) { return integrator->radiance(ray); });
    done++;

    if (done == target) {
      target = next_sample_target(done);
      std::cout << std::fixed;
      std::cout << std::setprecision(3);
      std::cout << std::setw(7);
      std::cout << target << std::setw(5)
                << " | est. time: " << estimate_seconds_left(done, timer.elapsed_seconds(), target)
                << " s, " << timer.elapsed_seconds() << " s total\n";
    }

    if (break_loop)
      break;
  }

  auto time = timer.elapsed_seconds();

  std::cout << "Did " << done << " samples in " << time << " seconds\n";

  delete integrator; // shaddap clang-tidy

  auto image = Image(film);

  std::stringstream ss;
  ss << "images/sphere2_";
  ss << done;
  ss << ".png";

  image.extended_reinhard().gamma(2.2).clamp().write_png(ss.str().c_str());

  return 0;
}
