#pragma once

#include "renderer/integrators/integrator.hpp"

namespace Oxy::Integrators {

  /*
    My idea is:

    PSSMLT at each vertex of the light path requires one or more values for determining
    what bounce type (transmission, clearcoat, diffuse), and what parameters to use
    to calculate the bounce direction.

    For example:
    A diffuse bounce requires 3 parameters. 1st the bounce type itself, and then 2 for phi
    and theta which is used to compute the bounce direction vector.

    However, a transmissive bounce only requires two, the bounce type, and the number used
    to determine wether it should reflect or refract (shlicks approximation).

    So the idea is to store an array of variants for each vertex in the light path, with
    a templated bounce type and required number of parameters.

    Turns out i was overthinking it with the templating, way easier to just store a fixed
    amount of params per vertex.
  */

  namespace PSSMLT {

    constexpr std::size_t NumVertexParams = 4;

    class Vertex {
    public:
      template <std::size_t ParamIndex>
      auto get() const {
        return m_params[ParamIndex];
      }

      void reset() {
        for (std::size_t i = 0; i < NumVertexParams; i++)
          m_params[i] = random<FloatType>(0, 1);

        m_contribution = 0.0;
      }

      bool mutate(FloatType contribution) {
        // i have no idea what im doing lmao

        if (m_contribution == 0.0)
          m_contribution = contribution;
        else
          m_contribution += (contribution - m_contribution) * 0.125;

        auto a = glm::exp(-m_contribution);

        if (random<FloatType>(0, 1) < a) {
          reset();
          return true;
        }
        else {
          constexpr FloatType s1 = 1.0 / 1024.0;
          constexpr FloatType s2 = 1.0 / 64.0;

          static const FloatType log = -glm::log(s2 / s1);

          for (std::size_t i = 0; i < NumVertexParams; i++) {
            const FloatType dv = s2 * glm::exp(log * random<FloatType>(0, 1));

            auto& value = m_params[i];
            if (random<FloatType>(0, 1) < 0.5) {
              value += dv;
              if (value > 1)
                value -= 1;
            }
            else {
              value -= dv;
              if (value < 0)
                value += 1;
            }
          }

          return false;
        }
      }

    private:
      FloatType m_params[NumVertexParams];
      FloatType m_contribution;
    };

    class Path {
    public:
      Path() = default;
      Path(int max_segments) {
        m_path.reserve(max_segments);

        for (int i = 0; i < max_segments; i++) {
          m_path.push_back(Vertex());
          m_path.back().reset();
        }
      }

      auto& vertex(int index) { return m_path[index]; }

    private:
      std::vector<Vertex> m_path;
    };

  } // namespace PSSMLT

  class PSSMLTIntegrator final : public Integrator {
  public:
    PSSMLTIntegrator(int max_bounces = 8)
        : m_max_bounces(max_bounces) {}

    virtual Color radiance(const SingleRay& ray, int x, int y) override {
      auto& path = get_pixel_path(x, y);

      SingleRay current_ray = ray;

      Color color(0.0);
      Color throughput(1.0);

      for (int n = 0; n < m_max_bounces; n++) {
        auto isect = m_world.intersect_ray(current_ray);

        if (!isect.hit) {
          color += throughput * m_world.sample_environment(current_ray);

          if (n < 3)
            path.vertex(0).reset();

          break;
        }

        auto& vertex = path.vertex(n);

        auto& obj   = isect.object;
        auto& mater = obj->material();

        auto scatter =
            mater->scatter_impl(isect, vertex.get<0>(), vertex.get<1>(), vertex.get<2>());

        color += throughput * scatter.energy *
                 mater->pdf(current_ray.direction, isect.hitnormal, scatter.ray.direction);

        throughput *= scatter.absorption;
        current_ray = scatter.ray;

        auto p = throughput.max();
        if (vertex.get<3>() > p)
          break;

        throughput *= 1.0 / p;
      }

      for (int n = 0; n < m_max_bounces; n++) {
        auto& vertex = path.vertex(n);
        vertex.mutate(1.0 * color.luminance());
      }

      return color;
    }

    virtual void setup(const IntegratorSetupContext& ctx) override {
      m_width  = ctx.width;
      m_height = ctx.height;
      m_pixel_paths.reserve(m_width * m_height);

      for (int i = 0; i < m_width * m_height; i++)
        m_pixel_paths.push_back(PSSMLT::Path(m_max_bounces));
    }

  private:
    PSSMLT::Path& get_pixel_path(int x, int y) { return m_pixel_paths[x + y * m_width]; }

    int m_max_bounces;

    int                       m_width{}, m_height{};
    std::vector<PSSMLT::Path> m_pixel_paths;
  };

} // namespace Oxy::Integrators
