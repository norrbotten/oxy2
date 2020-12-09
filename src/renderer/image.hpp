#pragma once

#include "renderer/sample_film.hpp"

namespace Oxy {

  /*
    Image class

    This is ment to be used when youre done sampling, and wanna filter the image and save

    TODO: reinhard with luminescence
  */
  class Image {
  public:
    Image(const SampleFilm& film);
    ~Image();

    Image& clamp(double min = 0.0, double max = 1.0); // Clamps all values between min and max

    Image& gamma(double gamma = 2.2); // Applies gamma correction

    Image& simple_reinhard();   // Applies a simple reinhard filter
    Image& extended_reinhard(); // Applies the extended reinhard filter

    // Applies a custom TMO (tone mapping operation)
    template <typename Callable>
    Image& tmo(Callable callback) {
      for_each_pixel(callback);
      update_minmax();
      return *this;
    }

    Color get(int x, int y) const {
      if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
        double r = m_buffer[3 * (x + y * m_width) + 0];
        double g = m_buffer[3 * (x + y * m_width) + 1];
        double b = m_buffer[3 * (x + y * m_width) + 2];
        return {r, g, b};
      }

      return Color();
    }

    void to_rgba_vector(std::vector<unsigned char>* buf) const;
    void write_png(const char* filename) const;

    CREF(width);
    CREF(height);

    CREF(min);
    CREF(max);

  private:
    void set(int x, int y, Color col) {
      if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
        m_buffer[3 * (x + y * m_width) + 0] = col.r();
        m_buffer[3 * (x + y * m_width) + 1] = col.g();
        m_buffer[3 * (x + y * m_width) + 2] = col.b();
      }
    }

    template <typename Callable>
    void for_each_pixel(Callable callback) {
      (void)callback;
      for (int y = 0; y < m_height; y++)
        for (int x = 0; x < m_width; x++)
          set(x, y, callback(x, y, get(x, y)));
    }

    void update_minmax();

  private:
    int m_width;
    int m_height;

    double* m_buffer;

    Color m_min;
    Color m_max;
  };

} // namespace Oxy