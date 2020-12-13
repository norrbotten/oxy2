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

  if (error.has_value())
    std::cout << error.value() << "\n";
}
