#include "renderer/material/texture.hpp"

namespace Oxy {

  Texture::~Texture() {
    if (m_buffer != nullptr)
      delete m_buffer;
  }

  bool Texture::load_from_file(fs::path path) {
    if (!fs::is_regular_file(path))
      return true;

    if (path.extension() == ".png") {
      std::vector<unsigned char> image;

      unsigned width, height;
      auto     err = lodepng::decode(image, width, height, path.c_str());

      if (err)
        return true;

      m_width  = width;
      m_height = height;

      if (m_buffer != nullptr)
        delete m_buffer;

      m_buffer = new Color[width * height];

      for (int i = 0; i < width * height; i++)
        m_buffer[i] = Color(image.at(i * 4 + 0), image.at(i * 4 + 1), image.at(i * 4 + 2));

      return false;
    }

    return true;
  }

  void Texture::make_solid_color(Color color) {
    if (m_buffer != nullptr)
      delete m_buffer;

    m_width  = 1;
    m_height = 1;

    m_buffer    = new Color[1];
    m_buffer[0] = color;
  }

  Color Texture::sample(FloatType x, FloatType y) const {
    auto read = [this](int x, int y, int ox = 0, int oy = 0) {
      auto read_x = (x + ox) % m_width;
      auto read_y = (y + oy) % m_height;
      return m_buffer[read_x + m_width * read_y];
    };

    x = std::fmod(x, 1.0);
    y = std::fmod(y, 1.0);

    // nearest neighbour for now
    return read(x * m_width, y * m_height);
  }

} // namespace Oxy
