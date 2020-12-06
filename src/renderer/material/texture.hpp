#pragma once

#include <filesystem>
#include <vector>

#include <lodepng.h>

#include "renderer/material/color.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  class Texture {
  public:
    ~Texture();

    bool load_from_file(fs::path path);
    void make_solid_color(Color color = Color(1.0, 1.0, 1.0));

    Color sample(FloatType x, FloatType y) const;

    const auto& buffer() const { return m_buffer; }
    const auto& width() const { return m_width; }
    const auto& height() const { return m_height; }

  private:
    Color*      m_buffer = nullptr;
    std::size_t m_width, m_height;
  };

} // namespace Oxy