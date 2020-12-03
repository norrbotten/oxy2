
#include <iostream>

#include "renderer/material/texture.hpp"

using namespace Oxy;

int main() {
  auto texture = Texture();
  if (texture.load_from_file("./data/grass.png")) {
    std::cout << "fucked\n";
  }
  else {
    auto col = texture.sample(0.5, 0.5);
    std::cout << "r: " << col.r() << "\ng: " << col.g() << "\nb: " << col.b() << "\n";
  }

  return 0;
}
