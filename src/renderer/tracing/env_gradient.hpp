#pragma once

#include "renderer/tracing/environment.hpp"

namespace Oxy::Tracing {

  class GradientEnvironment : public EnvironmentMap {
  public:
    GradientEnvironment(Color color_up, Color color_down, FloatType energy)
        : m_energy_up(color_up * energy)
        , m_energy_down(color_down * energy) {}

    virtual Color sample(const SingleRay& ray) const override {
      auto z = ray.direction.z * 0.5 + 0.5;
      return m_energy_up * z + m_energy_down * (1.0 - z);
    }

  private:
    Color m_energy_up, m_energy_down;
  };

} // namespace Oxy::Tracing
