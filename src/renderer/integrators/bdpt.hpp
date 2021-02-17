#pragma once

#include "renderer/integrators/integrator.hpp"

namespace Oxy::Integrators {

  namespace BDPT {

    struct LightPathVertex {
      IntersectionContext      isect;
      Color                    energy;
      Tracing::TracableObject* source_obj;

      bool is_environment_hit() const { return !isect.hit; }
    };

    constexpr auto MAX_LIGHT_PATH_VERTEX = 64;

  } // namespace BDPT

  /*
    Bidirectional integrator
  */
  class BDPTIntegrator final : public Integrator {
  public:
    BDPTIntegrator(int max_ray_bounce = 5, int max_light_bounce = 5)
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

        const auto& obj   = isect.object;
        const auto& mater = obj->material();

        const auto scatter = mater->scatter(isect);

        color += throughput * scatter.energy;

        throughput *= scatter.absorption;

        // sample a light path
        const auto [light_path, light_path_n] = light_random_walk();

        for (int i = 0; i < light_path_n; i++) {
          const auto& light_vertex = *(light_path + light_path_n);

          const auto& light_ray_isect = light_vertex.isect;
          auto to_light_vertex = m_world.intersect_line(isect.hitpos + isect.hitnormal * 1e-6,
                                                        light_ray_isect.ray.origin);

          // path is obscured
          if (!to_light_vertex.hit || light_vertex.source_obj != to_light_vertex.object)
            continue;

          // connect the paths
          const auto light_mater = light_ray_isect.object->material();

          /* clang-format off */
          auto contribution = throughput *
                              light_vertex.energy *
                              mater->pdf(current_ray.direction, isect.hitnormal, to_light_vertex.ray.direction) *
                              light_mater->pdf(light_ray_isect.ray.direction, light_ray_isect.hitnormal, -to_light_vertex.ray.direction) /
                              (1.0 + light_ray_isect.t * light_ray_isect.t);
          /* clang-format on */

          color += contribution;
        }

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
    std::pair<const BDPT::LightPathVertex* const, int> light_random_walk() const {
      static thread_local std::array<BDPT::LightPathVertex, BDPT::MAX_LIGHT_PATH_VERTEX> result;

      // pick a random light source
      // (this would need to be weighed by intesity, surface area, etc.. for no bias to occur)
      auto n      = random<int>(0, m_light_sources.size() - 1);
      auto source = m_light_sources[n];

      SingleRay current_ray = source->random_ray_from_surface();

      Color color  = source->material()->albedo();
      Color energy = source->material()->emission_energy();

      int i;
      for (i = 0; i < m_max_light_bounce; i++) {
        auto isect = m_world.intersect_ray(current_ray);

        if (!isect.hit)
          break;

        auto& obj   = isect.object;
        auto& mater = obj->material();

        auto scatter = mater->scatter(isect);

        color *= scatter.absorption;
        energy += scatter.energy;

        current_ray = scatter.ray;

        result[i] = BDPT::LightPathVertex{isect, color * energy, source};
      }

      return {result.data(), i};
    }

  private:
    std::vector<Tracing::TracableObject*> m_light_sources;

    int m_max_ray_bounce, m_max_light_bounce;
  };

} // namespace Oxy::Integrators
