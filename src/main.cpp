#include <iomanip>
#include <iostream>
#include <streambuf>
#include <string>

#include <omp.h>
#include <signal.h>

#include "renderer/renderer.hpp"

int main() {

  Oxy::Renderer renderer;

  auto error = renderer.load_file("scenes/two_spheres.sdl");

  if (error.has_value()) {
    std::cout << error.value() << "\n";
    return 1;
  }

  renderer.film().resize(1024, 1024);

  renderer.camera().set_pos(Oxy::Vec3(-6, 0, 4));
  renderer.camera().aim(Oxy::Vec3(0));
  renderer.camera().set_fov(60);

  for (int i = 0; i < 512; i++)
    renderer.sample_once();

  renderer.save_png("data/two_spheres.png");
}
