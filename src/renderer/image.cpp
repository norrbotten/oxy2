#include "renderer/image.hpp"

namespace Oxy {

  Image::Image(const SampleFilm& film)
      : m_width(film.width())
      , m_height(film.height()) {

    m_buffer = new double[3 * m_width * m_height]();

    for (int y = 0; y < m_height; y++)
      for (int x = 0; x < m_width; x++) {
        auto col = film.get(x, y);

        m_buffer[3 * (x + y * m_width) + 0] = col.r();
        m_buffer[3 * (x + y * m_width) + 1] = col.g();
        m_buffer[3 * (x + y * m_width) + 2] = col.b();
      }

    update_minmax();
  }

  Image::~Image() {
    if (m_buffer != nullptr)
      delete m_buffer;
  }

  Image& Image::clamp(double min, double max) {
    for_each_pixel([min, max](int, int, Color col) {
      return Color(glm::clamp(col.r(), min, max), glm::clamp(col.g(), min, max),
                   glm::clamp(col.b(), min, max));
    });

    update_minmax();
    return *this;
  }

  Image& Image::gamma(double gamma) {
    auto f = 1.0 / gamma;
    for_each_pixel([f](int, int, Color col) {
      return Color(glm::pow(col.r(), f), glm::pow(col.g(), f), glm::pow(col.b(), f));
    });

    update_minmax();
    return *this;
  }

  Image& Image::simple_reinhard() {
    for_each_pixel([](int, int, Color col) { return col / (col + 1.0); });
    update_minmax();
    return *this;
  }

  Image& Image::extended_reinhard() {
    for_each_pixel([this](int, int, Color col) {
      auto max = this->max();
      auto num = col * (col / (max * max) + 1.0);
      return num / (col + 1.0);
    });
    update_minmax();
    return *this;
  }

  void Image::update_minmax() {
    Color min(std::numeric_limits<double>::max());
    Color max(std::numeric_limits<double>::lowest());

    for_each_pixel([&min, &max](int, int, Color col) {
      min.r() = glm::min(min.r(), col.r());
      min.g() = glm::min(min.g(), col.g());
      min.b() = glm::min(min.b(), col.b());

      max.r() = glm::max(max.r(), col.r());
      max.g() = glm::max(max.g(), col.g());
      max.b() = glm::max(max.b(), col.b());

      return col;
    });

    m_min = min;
    m_max = max;
  }

  void Image::to_rgba_vector(std::vector<unsigned char>* buf) const {
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

  void Image::write_png(const char* filename) const {
    std::vector<unsigned char> buf;
    this->to_rgba_vector(&buf);

    auto err = lodepng::encode(filename, buf, m_width, m_height);

    if (err)
      std::cout << lodepng_error_text(err);
  }

} // namespace Oxy
