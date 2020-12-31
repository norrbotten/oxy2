#pragma once

#include "renderer/tracing/environments/nishita.hpp"

namespace Oxy::Tracing {

  // the earth is huge, the camera not so.
  class PreloadedNishitaEnvironment : public EnvironmentMap {
  public:
    PreloadedNishitaEnvironment(const NishitaParameters& parameters = {}, int res = 1024)
        : m_resolution(res) {

      m_map = new Color[res * res]();

      auto env = NishitaEnvironment(parameters);

      for (int y = 0; y < res; y++)
        for (int x = 0; x < res; x++) {
          Vec3 dir{};

          auto angle = ((FloatType)x / res) * 2 * M_PI;
          dir.x      = glm::cos(angle);
          dir.y      = glm::sin(angle);

          dir.z = ((FloatType)y / res) * 2.0 - 1.0;

          dir = glm::normalize(dir);

          m_map[x + y * res] = env.sample(SingleRay{Vec3{0, 0, 0}, dir});
        }
    }

    ~PreloadedNishitaEnvironment() {
      if (m_map != nullptr)
        delete[] m_map;
    }

    virtual Color sample(const SingleRay& ray) const override {
      auto angle = std::atan2(ray.direction.y, ray.direction.x);

      int x = int((angle / (2 * M_PI)) * m_resolution) % m_resolution;
      int y = int((ray.direction.z * 0.5 + 0.5) * m_resolution) % m_resolution;

      return m_map[x + y * m_resolution];
    }

  private:
    Color* m_map;
    int    m_resolution;
  };

} // namespace Oxy::Tracing