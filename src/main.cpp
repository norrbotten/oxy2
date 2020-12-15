#include <iomanip>
#include <iostream>
#include <streambuf>
#include <string>

#include <omp.h>
#include <signal.h>

#include "renderer/renderer.hpp"

int main() {

  Oxy::Renderer renderer;

  auto error = renderer.load_file("scenes/glass.sdl");

  if (error.has_value()) {
    std::cout << error.value() << "\n";
    return 1;
  }

  for (int i = 0; i < renderer.max_samples(); i++) {
    std::cout << i << "\n";
    renderer.sample_once();
  }

  renderer.save_png("images/glass.png");
}
