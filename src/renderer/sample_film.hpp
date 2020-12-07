#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include <lodepng.h>

#include "renderer/material/color.hpp"

namespace Oxy {

  class SampleFilm final {
  public:
    SampleFilm();
    ~SampleFilm();

    auto width() const { return m_width; }
    auto height() const { return m_height; }

    void resize(int width, int height);
    void splat(int x, int y, double r, double g, double b);
    void splat(int x, int y, const Color& sample);

    Color get(int x, int y) const;

    void copy_to_rgba_buffer(unsigned char* buffer) const;

    // what the fuck when i do reference the buffer is fucking empty when this
    // method leaves scope, but pointer is fine
    void to_rgba_vector(std::vector<unsigned char>* buf) const;

    void clear();

    void write_png(const char* filename);

  private:
    Color get_cumulative(int x, int y) const {
      return {m_cumulative_buffer[3 * (x + y * m_width) + 0],
              m_cumulative_buffer[3 * (x + y * m_width) + 1],
              m_cumulative_buffer[3 * (x + y * m_width) + 2]};
    }
    unsigned int get_samples(int x, int y) const { return m_sample_count[x + y * m_width]; }

  private:
    int           m_width, m_height;
    double*       m_cumulative_buffer;
    unsigned int* m_sample_count;
  };

} // namespace Oxy
