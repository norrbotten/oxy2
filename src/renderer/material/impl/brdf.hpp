#pragma once

#include <filesystem>

#include "renderer/material/material.hpp"
#include "renderer/material/texture.hpp"

#include "renderer/tracing/utils.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  class BRDF final : public Material {
  public:
    BRDF() {
      m_texture.make_solid_color();
      m_albedo = Color(1.0, 1.0, 1.0);
    }

    BRDF(Color color) {
      m_texture.make_solid_color(color);
      m_albedo = Color(1.0, 1.0, 1.0);
    }

    BRDF(fs::path texture_path, Color albedo = Color(1.0, 1.0, 1.0)) {
      if (m_texture.load_from_file(texture_path))
        m_texture.make_solid_color();

      m_albedo = albedo;
    }

    Color sample(const IntersectionContext& ctx) override {
      return m_texture.sample(ctx.uv.x, ctx.uv.y);
    }

    SingleRay reflect(const IntersectionContext& ctx) override {}

    void set_reflectivity(FloatType val) { m_reflectivity = val; }

    void set_diffusivity(FloatType val) { m_diffusivity = val; }

    void normalize_params() {
      auto sum = m_reflectivity + m_diffusivity;
      m_reflectivity /= sum;
      m_diffusivity /= sum;
    }

  private:
    Texture m_texture;

    Color m_albedo;

    FloatType m_reflectivity;
    FloatType m_diffusivity;
  };

} // namespace Oxy
