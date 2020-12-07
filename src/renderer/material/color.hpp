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

    auto max() const { return glm::max(m_r, glm::max(m_g, m_b)); }

    REF(r);
    REF(g);
    REF(b);

    CREF(r);
    CREF(g);
    CREF(b);

    Color operator+(const Color& rhs) { return Color{r() + rhs.r(), g() + rhs.g(), b() + rhs.b()}; }
    Color operator+(FloatType rhs) { return Color{r() + rhs, g() + rhs, b() + rhs}; }

    Color operator*(const Color& rhs) { return Color{r() * rhs.r(), g() * rhs.g(), b() * rhs.b()}; }
    Color operator*(FloatType rhs) { return Color{r() * rhs, g() * rhs, b() * rhs}; }

    Color operator/(const Color& rhs) { return Color{r() / rhs.r(), g() / rhs.g(), b() / rhs.b()}; }
    Color operator/(FloatType rhs) { return Color{r() / rhs, g() / rhs, b() / rhs}; }

    Color& operator+=(const Color& rhs) {
      m_r += rhs.r();
      m_g += rhs.g();
      m_b += rhs.b();
      return *this;
    }

    Color& operator+=(FloatType rhs) {
      m_r += rhs;
      m_g += rhs;
      m_b += rhs;
      return *this;
    }

    Color& operator*=(const Color& rhs) {
      m_r *= rhs.r();
      m_g *= rhs.g();
      m_b *= rhs.b();
      return *this;
    }

    Color& operator*=(FloatType rhs) {
      m_r *= rhs;
      m_g *= rhs;
      m_b *= rhs;
      return *this;
    }

    Color& operator/=(const Color& rhs) {
      m_r /= rhs.r();
      m_g /= rhs.g();
      m_b /= rhs.b();
      return *this;
    }

    Color& operator/=(FloatType rhs) {
      m_r /= rhs;
      m_g /= rhs;
      m_b /= rhs;
      return *this;
    }

  private:
    FloatType m_r, m_g, m_b;
  };

} // namespace Oxy
