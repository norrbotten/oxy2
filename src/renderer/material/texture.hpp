#pragma once

#include <filesystem>
#include <vector>

#include <lodepng.h>

#include "renderer/material/color.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  class Texture {
  public:
    Texture();
    ~Texture();

    bool load_from_file(fs::path path) {
      if (!fs::is_regular_file(path))
        return true;

      if (path.extension() == "png") {
        std::vector<unsigned char> image;

        unsigned width, height;
        auto     err = lodepng::decode(image, width, height, path.c_str());

        if (err)
          return true;

        if (m_buffer != nullptr)
          delete m_buffer;

        m_buffer = new Color[width * height];

        for (int i = 0; i < width * height; i++)
          m_buffer[i] = Color(image.at(i * 4 + 0), image.at(i * 4 + 1), image.at(i * 4 + 2));

        return false;
      }

      return true;
    }

    const auto& buffer() const { return m_buffer; }
    const auto& width() const { return m_width; }
    const auto& height() const { return m_height; }

  private:
    Color*      m_buffer = nullptr;
    std::size_t m_width, m_height;
  };

} // namespace Oxy