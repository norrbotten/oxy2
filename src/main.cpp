#include <iomanip>
#include <iostream>
#include <streambuf>
#include <string>

#include <omp.h>
#include <signal.h>

#include "renderer/timer.hpp"

#include "renderer/camera.hpp"
#include "renderer/image.hpp"
#include "renderer/sample_film.hpp"

#include "renderer/integrators/naive.hpp"
#include "renderer/integrators/preview.hpp"

#include "renderer/material/materials.hpp"
#include "renderer/tracing/sdf.hpp"
#include "renderer/tracing/sphere.hpp"

#include "renderer/sdl/parser.hpp"

using namespace Oxy;

static bool break_loop = false;

int main2() {

  signal(
      SIGINT, *[](int) {
        std::cout << "breaking loop..\n";
        break_loop = true;
      });

  auto film = SampleFilm();
  film.resize(512, 512);

  auto cam = Camera();
  cam.set_pos(Vec3(-10, 0, 0));
  cam.set_fov(50);
  cam.aim(Vec3(0, 0, 0));

  auto lambert_white = create_lambertian(Color(1.0));
  auto lambert_red   = create_lambertian(Color(1.0, 0.0, 0.0));
  auto lambert_green = create_lambertian(Color(0.0, 1.0, 0.0));
  auto emissive      = create_emissive(Color(5.0));

  auto left_wall        = new TracableSphere(Vec3(0.0, -1e6 - 4, 0.0), 1e6);
  left_wall->material() = lambert_red;

  auto right_wall        = new TracableSphere(Vec3(0.0, 1e6 + 4, 0.0), 1e6);
  right_wall->material() = lambert_green;

  auto front_wall        = new TracableSphere(Vec3(1e6 + 4, 0.0, 0.0), 1e6);
  front_wall->material() = lambert_white;

  auto top_wall        = new TracableSphere(Vec3(0.0, 0.0, 1e6 + 4), 1e6);
  top_wall->material() = lambert_white;

  auto bottom_wall        = new TracableSphere(Vec3(0.0, 0.0, -1e6 - 4), 1e6);
  bottom_wall->material() = lambert_white;

  auto lamp        = new TracableSphere(Vec3(0.0, 0.0, 9.0), 5.3);
  lamp->material() = emissive;

  auto sdf = new TracableSDF(Vec3(), 1.0, [](const Vec3& point) {
    return SDFPos::scale(point, 2.0, [&](const Vec3& p) {
      auto transform = glm::rotate(M_PI / 3, Vec3(1, 0, 1));
      return SDFPos::transform(p, transform, SDFShape::torus, Vec2(1.0, 0.5));
    });
  });

  sdf->material() = lambert_white;

  auto integrator = new Integrators::Naive();

  integrator->world().add_object(left_wall);
  integrator->world().add_object(right_wall);
  integrator->world().add_object(front_wall);
  integrator->world().add_object(top_wall);
  integrator->world().add_object(bottom_wall);
  integrator->world().add_object(lamp);

  integrator->world().add_object(sdf);

  Timer timer;

  int done   = 0;
  int target = next_sample_target(done);

  timer.start();

  while (done < 10) {
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
  ss << "images/cornell1_";
  ss << done;
  ss << ".png";

  image.extended_reinhard().gamma(2.2).clamp().write_png(ss.str().c_str());

  return 0;
}

int main() {
  std::ifstream f("data/sdl_example.txt");
  std::string   str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

  auto parser    = SDL::Parser(str);
  auto scenedecl = parser.parse();

  if (scenedecl != nullptr) {
    std::cout << scenedecl->stringify();
  }
}
