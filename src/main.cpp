#include <iomanip>
#include <iostream>
#include <streambuf>
#include <string>

#include <signal.h>

#include "renderer/renderer.hpp"

#include "renderer/parsers/stl.hpp"
#include "renderer/tracing/mesh.hpp"

#include "renderer/netrender/httplib/http_server.hpp"

int main() {

  using namespace Oxy;

  Renderer renderer;

  auto error = renderer.load_file("scenes/ground_plane.sdl");

  if (error.has_value()) {
    std::cout << error.value() << "\n";
    return 1;
  }

  std::vector<Primitive::TrianglePrimitive> dragon;
  std::cout << "Parse: " << Parsers::parse_stl("data/dragon.stl", dragon) << "\n";

  std::cout << dragon.size() << "\n";

  auto dragon_obj        = new Tracing::TracableMesh(dragon);
  dragon_obj->material() = renderer.material_pool().get("frosted glass").value();

  renderer.world().add_object(dragon_obj);

  renderer.setup_integrator();

  for (int i = 0; i < renderer.max_samples(); i++) {
    std::cout << i << "\n";
    renderer.sample_once();
  }

  renderer.save_png("images/pssmlt_test1.png");
}
