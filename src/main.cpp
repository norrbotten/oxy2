#include <iomanip>
#include <iostream>
#include <streambuf>
#include <string>

#include <signal.h>

#include "renderer/renderer.hpp"

#include "renderer/parsers/stl.hpp"
#include "renderer/tracing/mesh.hpp"

#include "renderer/netrender/httplib/http_server.hpp"

#include "renderer/tracing/environments/flat.hpp"

static bool break_loop = false;

int main() {

  signal(
      SIGINT, +[](int) {
        break_loop = true;
        std::cout << "breaking loop prematurely...\n";
      });

  using namespace Oxy;

  Renderer renderer;

  renderer.world().set_environment(new Tracing::FlatEnvironment(Color{0.0, 0.0, 0.0}));

  /*
  std::cout << "Loading file..\n";
  auto error = renderer.load_file("scenes/ground_plane.sdl");

  if (error.has_value()) {
    std::cout << error.value() << "\n";
    return 1;
  }

  {
    std::cout << "Loading mesh..\n";
    std::vector<Primitive::TrianglePrimitive> mesh;
    std::cout << "Parse: " << Parsers::parse_stl("data/ring.stl", mesh) << "\n";

    std::cout << mesh.size() << " triangles\n";

    std::cout << "Building BVH..\n";
    auto obj        = new Tracing::TracableMesh(mesh);
    obj->material() = renderer.material_pool().get("gold").value();

    renderer.world().add_object(obj);
  }
  */

  std::cout << "Loading file..\n";
  auto error = renderer.load_file("scenes/cornellbox.sdl");

  if (error.has_value()) {
    std::cout << error.value() << "\n";
    return 1;
  }

  {
    std::cout << "Loading mesh..\n";
    std::vector<Primitive::TrianglePrimitive> mesh;
    std::cout << "Parse: " << Parsers::parse_stl("data/bigmonke.stl", mesh) << "\n";

    std::cout << mesh.size() << " triangles\n";

    std::cout << "Building BVH..\n";
    auto obj        = new Tracing::TracableMesh(mesh);
    obj->material() = renderer.material_pool().get("glass").value();

    renderer.world().add_object(obj);
  }

  std::cout << "Setting up integrator..\n";
  renderer.setup_integrator();

  std::cout << "Rendering..\n";

  Timer timer;
  timer.start();

  for (int i = 0; i < renderer.max_samples(); i++) {
    renderer.sample_once();

    auto seconds_passed = timer.elapsed_seconds();
    auto seconds_left   = estimate_seconds_left(i + 1, seconds_passed, renderer.max_samples());

    std::cout << "sample " << i << " - est. time left: " << format_time(seconds_left)
              << " - time passed: " << format_time(seconds_passed) << "\n";

    if (break_loop) {
      break;
    }
  }

  std::cout << "Render done. writing png\n";

  renderer.save_png("images/bong4.png");
}
