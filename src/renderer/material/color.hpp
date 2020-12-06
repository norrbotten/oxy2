#pragma once

#include "renderer/types.hpp"

namespace Oxy {

  struct Color {
  public:
    Color()
        : m_r(0)
        , m_g(0)
        , m_b(0) {}

    Color(FloatType val)
        : m_r(val)
        , m_g(val)
        , m_b(val) {}

    Color(FloatType r, FloatType g, FloatType b)
        : m_r(r)
        , m_g(g)
        , m_b(b) {}

    Color(unsigned char val)
        : m_r((FloatType)val / 255.0)
        , m_g((FloatType)val / 255.0)
        , m_b((FloatType)val / 255.0) {}

    Color(unsigned char r, unsigned char g, unsigned char b)
        : m_r((FloatType)r / 255.0)
        , m_g((FloatType)g / 255.0)
        , m_b((FloatType)b / 255.0) {}

    void to_chars(unsigned char& r, unsigned char& g, unsigned char& b) {
      r = (unsigned char)(255 * m_r);
      g = (unsigned char)(255 * m_g);
      b = (unsigned char)(255 * m_b);
    }

    const auto& r() const { return m_r; }
    const auto& g() const { return m_g; }
    const auto& b() const { return m_b; }

  private:
    FloatType m_r, m_g, m_b;
  };

} // namespace Oxy
