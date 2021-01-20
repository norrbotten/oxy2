#pragma once

#include "renderer/integrators/integrator.hpp"

namespace Oxy::Integrators {

  namespace BDPT {

    struct LightPathVertex {
      IntersectionContext isect;
      Color               energy;

      bool is_environment_hit() const { return !isect.hit; }
    };

  } // namespace BDPT

  /*
    Bidirectional integrator
  */
  class BDPTIntegrator final : public Integrator {
  public:
    BDPTIntegrator(int max_ray_bounce = 8, int max_light_bounce = 8)
        : m_max_ray_bounce(max_ray_bounce)
        , m_max_light_bounce(max_light_bounce) {}

    virtual Color radiance(const SingleRay& ray, int, int) override {
      SingleRay current_ray = ray;

      Color color(0.0);
      Color throughput(1.0);

      for (int i = 0; i < m_max_ray_bounce; i++) {
        auto isect = m_world.intersect_ray(current_ray);

        if (!isect.hit) {
          color += throughput * m_world.sample_environment(current_ray);
          break;
        }

        auto& obj   = isect.object;
        auto& mater = obj->material();

        auto scatter = mater->scatter(isect);

        color += throughput * scatter.energy;

        throughput *= scatter.absorption;

        // sample a light path
        // for (const auto& light_vertex : light_random_walk()) {
        //}

        // russian roulette ray termination
        auto p = throughput.max();
        if (random<FloatType>(0, 1) > p)
          break;

        throughput *= 1.0 / p;

        current_ray = scatter.ray;
      }

      return color;
    }

    virtual void setup(const IntegratorSetupContext&) override {
      // collect light emitting stuff from the scene
      for (auto& obj : m_world.objects()) {
        if (obj->material()->emission_energy().luminance() > 1e-6)
          m_light_sources.push_back(obj.get()); // cheating a bit here by stealing an unique ptr
      }
    }

  private:
    std::vector<BDPT::LightPathVertex> light_random_walk() const {
      // we can probably just write this into some thread local buffer since its got a max size
      // would provide the benefit of not having to dynamically allocate stuff
      std::vector<BDPT::LightPathVertex> result;
      result.reserve(m_max_light_bounce);

      // pick a random light source
      // (this would need to be weighed by intesity, surface area, etc.. for no bias to occur)
      auto n      = random<int>(0, m_light_sources.size() - 1);
      auto source = m_light_sources[n];

      SingleRay current_ray = source->random_ray_from_surface();

      Color color  = source->material()->albedo();
      Color energy = source->material()->emission_energy();

      for (int i = 0; i < m_max_light_bounce; i++) {
        auto isect = m_world.intersect_ray(current_ray);

        if (!isect.hit)
          break;

        auto& obj   = isect.object;
        auto& mater = obj->material();

        auto scatter = mater->scatter(isect);

        color *= scatter.absorption;
        energy += scatter.energy;

        current_ray = scatter.ray;

        result.push_back(BDPT::LightPathVertex{isect, color * energy});
      }

      return result;
    }

  private:
    std::vector<Tracing::TracableObject*> m_light_sources;

    int m_max_ray_bounce, m_max_light_bounce;
  };

} // namespace Oxy::Integrators
