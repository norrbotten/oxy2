#include "renderer/sample_film.hpp"

namespace Oxy {

  SampleFilm::SampleFilm()
      : m_width(0)
      , m_height(0)
      , m_cumulative_buffer(nullptr)
      , m_sample_count(nullptr) {}

  SampleFilm::~SampleFilm() {
    if (m_cumulative_buffer != nullptr)
      delete[] m_cumulative_buffer;

    if (m_sample_count != nullptr)
      delete[] m_sample_count;
  }

  void SampleFilm::resize(int width, int height) {
    if (width > 0 && height > 0 && width < 16384 && height < 16384) {
      m_width  = width;
      m_height = height;

      if (m_cumulative_buffer != nullptr)
        delete[] m_cumulative_buffer;

      if (m_sample_count != nullptr)
        delete[] m_sample_count;

      m_cumulative_buffer = new double[3 * width * height]();
      m_sample_count      = new unsigned int[width * height]();
    }
  }

  void SampleFilm::splat(int x, int y, double r, double g, double b) {
    if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
      auto ofs = 3 * (x + y * m_width);

      m_cumulative_buffer[ofs + 0] += r;
      m_cumulative_buffer[ofs + 1] += g;
      m_cumulative_buffer[ofs + 2] += b;

      m_sample_count[x + y * m_width]++;
    }
  }

  void SampleFilm::splat(int x, int y, const Color& col) { splat(x, y, col.r(), col.g(), col.b()); }

  Color SampleFilm::get(int x, int y) const {
    if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
      auto num_samples = m_sample_count[x + y * m_width];
      if (num_samples == 0)
        return Color();

      double r = m_cumulative_buffer[3 * (x + y * m_width) + 0] / num_samples;
      double g = m_cumulative_buffer[3 * (x + y * m_width) + 1] / num_samples;
      double b = m_cumulative_buffer[3 * (x + y * m_width) + 2] / num_samples;

      return {r, g, b};
    }

    return Color();
  }

  void SampleFilm::copy_to_rgba_buffer(unsigned char* buffer) const {
    for (int y = 0; y < m_height; y++)
      for (int x = 0; x < m_width; x++) {
        unsigned char r, g, b, a = (char)255;
        auto          col = get(x, y);
        col.to_chars(r, g, b);

        *(buffer + 0) = r;
        *(buffer + 1) = g;
        *(buffer + 2) = b;
        *(buffer + 3) = a;

        buffer += 4;
      }
  }

  void SampleFilm::to_rgba_vector(std::vector<unsigned char>* buf) const {
    for (int y = 0; y < m_height; y++)
      for (int x = 0; x < m_width; x++) {
        unsigned char r, g, b;
        auto          col = get(x, y);

        col.to_chars(r, g, b);

        buf->push_back(r);
        buf->push_back(g);
        buf->push_back(b);
        buf->push_back(255);
      }
  }

  void SampleFilm::clear() {
    if (m_cumulative_buffer != nullptr)
      for (int i = 0; i < 3 * m_width * m_height; i++)
        m_cumulative_buffer[i] = 0.0;

    if (m_sample_count != nullptr)
      for (int i = 0; i < m_width * m_height; i++)
        m_sample_count[i] = 0;
  }

  void SampleFilm::write_png(const char* filename) {
    if (m_cumulative_buffer == nullptr || m_sample_count == nullptr)
      return;

    std::vector<unsigned char> buf;
    this->to_rgba_vector(&buf);

    auto err = lodepng::encode(filename, buf, m_width, m_height);

    if (err)
      std::cout << lodepng_error_text(err);
  }

} // namespace Oxy
