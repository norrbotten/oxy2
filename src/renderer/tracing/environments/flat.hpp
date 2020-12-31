#pragma once

#include "renderer/tracing/environment.hpp"

namespace Oxy::Tracing {

  class FlatEnvironment : public EnvironmentMap {
  public:
    FlatEnvironment(Color color, FloatType energy = 0.02)
        : m_energy(color * energy) {}

    virtual Color sample(const SingleRay&) const override { return m_energy; }

  private:
    Color m_energy;
  };

} // namespace Oxy::Tracing
