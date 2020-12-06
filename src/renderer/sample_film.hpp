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

    Color get(int x, int y, double exposure = 1.0) const;

    void copy_to_rgba_buffer(unsigned char* buffer, double exposure = 1.0) const {
      for (int y = 0; y < m_height; y++)
        for (int x = 0; x < m_width; x++) {
          unsigned char r, g, b, a = (char)255;
          auto          col = get(x, y, exposure);
          col.to_chars(r, g, b);

          *(buffer + 0) = r;
          *(buffer + 1) = g;
          *(buffer + 2) = b;
          *(buffer + 3) = a;

          buffer += 4;
        }
    }

    // what the fuck when i do reference the buffer is fucking empty when this
    // method leaves scope, but pointer is fine
    void to_rgba_vector(std::vector<unsigned char>* buf, double exposure = 1.0) const {
      for (int y = 0; y < m_height; y++)
        for (int x = 0; x < m_width; x++) {
          unsigned char r, g, b;
          auto          col = get(x, y, exposure);

          col.to_chars(r, g, b);

          buf->push_back(r);
          buf->push_back(g);
          buf->push_back(b);
          buf->push_back(255);
        }
    }

    void clear() {
      if (m_cumulative_buffer != nullptr)
        for (int i = 0; i < 3 * m_width * m_height; i++)
          m_cumulative_buffer[i] = 0.0;

      if (m_sample_count != nullptr)
        for (int i = 0; i < m_width * m_height; i++)
          m_sample_count[i] = 0;
    }

    void write_png(const char* filename) {
      if (m_cumulative_buffer == nullptr || m_sample_count == nullptr)
        return;

      std::vector<unsigned char> buf;
      this->to_rgba_vector(&buf);

      auto err = lodepng::encode(filename, buf, m_width, m_height);

      if (err)
        std::cout << lodepng_error_text(err);
    }

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
