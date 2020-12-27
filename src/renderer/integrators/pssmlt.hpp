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
  */

  namespace PSSMLT {

    template <size_t NParams>
    class Vertex {
    public:
      template <size_t ParamIndex>
      auto get() const {
        return m_params[ParamIndex];
      }

      template <size_t ParamIndex>
      void set(FloatType value) {
        m_params[ParamIndex] = value;
      }

    private:
      FloatType m_params[NParams];

      FloatType m_illum;
    };

    class Path {
    public:
      Path() = default;
      Path(int max_segments) { m_path.reserve(max_segments); }

      using PathVertex = std::variant<Vertex<3>,  // diffuse
                                      Vertex<3>,  // clearcoat
                                      Vertex<2>>; // transmission

    private:
      std::vector<PathVertex> m_path;
    };

  } // namespace PSSMLT

  class PSSMLTIntegrator final : public Integrator {
  public:
    PSSMLTIntegrator(int max_bounces = 8)
        : m_max_bounces(max_bounces) {}

    virtual Color radiance(const SingleRay& ray, int x, int y) override {
      auto& path = get_pixel_path(x, y);

      (void)ray;
      (void)path;

      return Color();
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
