#pragma once

#include <vector>

#include "renderer/integrators/integrator.hpp"

namespace Oxy::Integrators {

  /*
    Like the naive integrator, but at each bounce we implicitly sample all light sources,
    resulting in direct lighting converging much faster
  */
  class NaiveImplicit : public Integrator {
  public:
    NaiveImplicit(int max_bounces = 6)
        : m_max_bounces(max_bounces) {}

    virtual Color radiance(const SingleRay& ray) override {
      SingleRay current_ray = ray;

      const static auto ambient_energy = Color(135.0 / 255.0, 206.0 / 255.0, 235.0 / 255.0);

      int   implicit_samples = 0;
      Color implicit_energy(0.0);

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

        // the actual implicit sampling
        if (scatter.energy.max() < 1e-6) {
          for (auto& light : m_light_sources) {
            auto light_isect =
                m_world.intersect_line(isect.hitpos, light->random_point_on_surface());

            if (light_isect.object == light) { // we hit the object

              // drew it out on paper, 70% sure its correct
              auto attenuation = 1.0 / (1.0 + light_isect.t * light_isect.t);
              implicit_energy +=
                  light_isect.object->material()->sample(light_isect) * throughput *
                  scatter.absorption *
                  mater->pdf(-light_isect.ray.direction, isect.hitnormal, -current_ray.direction) *
                  attenuation;
            }

            implicit_samples++;
          }
        }

        current_ray = scatter.ray;

        // russian roulette ray termination
        auto p = throughput.max();
        if (random<FloatType>(0, 1) > p)
          break;

        throughput *= 1.0 / p;
      }

      return (color + implicit_energy) / (2 + implicit_samples);
    }

    virtual void pre_pass() override {
      for (auto& obj : m_world.objects()) {
        if (obj->material()->emission_energy().max() > 0.0) // is the object emissive
          m_light_sources.push_back(obj.get());
      }
    }

  private:
    int m_max_bounces;

    std::vector<Tracing::TracableObject*> m_light_sources;
  };

} // namespace Oxy::Integrators
