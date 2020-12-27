#pragma once

#include "renderer/integrators/integrator.hpp"

namespace Oxy::Integrators {

  /*
    Naive pathtracing implementation

    Dumb and slow, but works and produces pretty pictures
  */
  class NaiveIntegrator final : public Integrator {
  public:
    NaiveIntegrator(int max_bounces = 16)
        : m_max_bounces(max_bounces) {}

    virtual Color radiance(const SingleRay& ray, int, int) override {
      SingleRay current_ray = ray;

      const static auto ambient_energy = Color(135.0 / 255.0, 206.0 / 255.0, 235.0 / 255.0) * 0.05;

      Color color(0.0);
      Color throughput(1.0);

      for (int n = 0; n < m_max_bounces; n++) {
        auto isect = m_world.intersect_ray(current_ray);

        if (!isect.hit) {
          color += throughput * ambient_energy;
          break;
        }

        auto& obj   = isect.object;
        auto& mater = obj->material();

        auto scatter = mater->scatter(isect);

        color += throughput * scatter.energy;

        throughput *= scatter.absorption;
        current_ray = scatter.ray;

        // russian roulette ray termination
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
