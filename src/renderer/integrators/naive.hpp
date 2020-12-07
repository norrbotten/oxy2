#pragma once

#include "renderer/integrators/integrator.hpp"

namespace Oxy::Integrators {

  class Naive final : public Integrator {
  public:
    Naive(int max_bounces = 16)
        : m_max_bounces(max_bounces) {}

    virtual Color radiance(const SingleRay& ray) override {
      SingleRay current_ray = ray;

      Color color(0.0);
      Color throughput(1.0);

      for (int n = 0; n < m_max_bounces; n++) {
        auto isect = m_world.intersect_ray(current_ray);

        if (!isect.hit)
          break;

        auto& obj   = isect.object;
        auto& mater = obj->material();

        auto scatter = mater->scatter(isect);

        if (mater->is_emissive())
          color += throughput * scatter.light;

        throughput *= scatter.light;
        current_ray = scatter.ray;

        auto p = throughput.max();

        if (random<FloatType>(0, 1) > p)
          break;

        throughput *= 1.0 / p;
      }

      return color;
    }

  private:
    int m_max_bounces;
  };

} // namespace Oxy::Integrators
