#pragma once

#include <random>

#include "renderer/sample_film.hpp"
#include "renderer/types.hpp"

namespace Oxy {

  class Camera {
  public:
    Camera()
        : m_dist(0.0, 1.0) {}

    void set_fov(double fov_in_degrees) {
      m_fov = 1.0 / glm::tan(M_PI / 180.0 * fov_in_degrees * 0.5);
    }

    void set_pos(Vec3 pos) { m_origin = pos; }

    void set_dir(Vec3 dir) {
      m_forward = dir;

      m_left = glm::abs(dir.z) > 0.999999 ? Vec3(0, 1, 0)
                                          : glm::normalize(glm::cross(m_forward, Vec3(0, 0, 1)));

      m_up = glm::cross(m_left, m_forward);
    }

    void aim(Vec3 pos) {
      auto dir = glm::normalize(pos - m_origin);
      set_dir(dir);
    }

    SingleRay get_ray(int x, int y, int width, int height) {
      auto aspect = (double)height / (double)width;

      auto tent_x = m_dist(m_re);
      auto tent_y = m_dist(m_re);

      auto xf = 2.0 * (((double)x + tent_x) / (double)width - 0.5);
      auto yf = 2.0 * aspect * (((double)y + tent_y) / (double)height - 0.5);

      auto dir = glm::normalize(m_forward * m_fov + m_left * xf - m_up * yf);

      return SingleRay{m_origin, dir};
    }

    template <typename Callable>
    void for_each_pixel(SampleFilm& film, Callable callback) {
#pragma omp parallel for
      for (int y = 0; y < film.height(); y++)
        for (int x = 0; x < film.width(); x++)
          film.splat(x, y, callback(get_ray(x, y, film.width(), film.height())));
    }

  private:
    Vec3 m_origin;

    Vec3 m_forward;
    Vec3 m_left;
    Vec3 m_up;

    double m_fov;

    std::uniform_real_distribution<double> m_dist;
    std::default_random_engine             m_re;
  };

} // namespace Oxy
