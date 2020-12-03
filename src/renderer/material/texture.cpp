#include "renderer/material/texture.hpp"

namespace Oxy {

  Texture::~Texture() {
    if (m_buffer != nullptr)
      delete m_buffer;
  }

} // namespace Oxy
