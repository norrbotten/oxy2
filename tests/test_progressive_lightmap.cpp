
#include "renderer/integrators/photonmapping/lightmap.hpp"

int main() {
  using namespace Oxy::Integrators;

  Photonmapping::ProgressiveLightmap lightmap(0.00000001, 10);
  lightmap.add_sample(Oxy::Vec3(0, 0, 0), Oxy::Color(1.0, 1.0, 1.0), 0.1);

  std::cout << lightmap.sample(Oxy::Vec3(-0.05, 0, 0), 0.2, 0.05) << "\n";
}
