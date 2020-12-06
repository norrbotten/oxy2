#pragma once

#include <filesystem>

#include "renderer/material/material.hpp"
#include "renderer/material/texture.hpp"

#include "renderer/tracing/utils.hpp"
#include "renderer/types.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  class BSDF final : public Material {
  public:
    BSDF() {
      m_texture.make_solid_color();
      m_albedo = Color(1.0, 1.0, 1.0);
    }

    BSDF(Color color) {
      m_texture.make_solid_color(color);
      m_albedo = Color(1.0, 1.0, 1.0);
    }

    BSDF(fs::path texture_path, Color albedo = Color(1.0, 1.0, 1.0)) {
      if (m_texture.load_from_file(texture_path))
        m_texture.make_solid_color();

      m_albedo = albedo;
    }

    Color sample(const IntersectionContext& ctx) override {
      return m_texture.sample(ctx.uv.x, ctx.uv.y);
    }

    LightRay scatter(const IntersectionContext& ctx) override {
      LightRay result;

      result.light = this->sample(ctx);

      auto reflected =
          m_roughness == 0.0
              ? reflect(ctx.ray.direction, ctx.hitnormal)
              : (m_roughness == 1.0 ? random_vector_on_hemisphere(ctx.hitnormal)
                                    : random_vector_on_cone(ctx.hitnormal, m_roughness * M_PI / 4));

      result.ray = SingleRay{ctx.hitpos + ctx.hitnormal * 1e-6, reflected};

      return result;
    }

    REF(texture);
    REF(roughness);
    REF(clearcoat);
    REF(clearcoat_roughness);
    REF(ior);
    REF(transmission);

  private:
    Texture m_texture;

    Color m_albedo;

    FloatType m_roughness;

    FloatType m_clearcoat;
    FloatType m_clearcoat_roughness;

    FloatType m_ior;
    FloatType m_transmission;
  };

} // namespace Oxy
