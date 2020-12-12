#pragma once

#include <filesystem>

#include "renderer/material/material.hpp"
#include "renderer/material/texture.hpp"

#include "renderer/pools/texture_pool.hpp"

#include "renderer/types.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  /*
    Generic BSDF material, not fully implemented yet

    TODO:
    Clearcoat
    Transmission
    Probability distribution function (importance sampling related thing)
  */
  class BSDF final : public Material {
  public:
    BSDF(const Pools::TexturePool& texpool) {
      m_texture = texpool.get("@default").value();
      m_albedo  = Color(1.0, 1.0, 1.0);
    }

    BSDF(const Pools::TexturePool& texpool, Color color) {
      m_texture = texpool.get("@default").value();
      m_albedo  = color;
    }

    void set_texture(const Pools::TexturePool& texpool, Pools::TextureHandle handle) {
      if (auto tex = texpool.get(handle); tex.has_value())
        m_texture = tex.value();
    }

    Color sample(const IntersectionContext& ctx) const override {
      return m_texture != nullptr ? m_texture->sample(ctx.uv.x, ctx.uv.y) : Color(1.0);
    }

    LightRay scatter(const IntersectionContext& ctx) const override {
      LightRay result;

      result.energy = this->sample(ctx);

      auto reflected =
          m_roughness == 0.0
              ? reflect(ctx.ray.direction, ctx.hitnormal)
              : (m_roughness == 1.0 ? random_vector_on_hemisphere(ctx.hitnormal)
                                    : random_vector_on_cone(ctx.hitnormal, m_roughness * M_PI / 4));

      result.ray = SingleRay{ctx.hitpos + ctx.hitnormal * 1e-6, reflected};

      return result;
    }

    REF(roughness);
    REF(clearcoat);
    REF(clearcoat_roughness);
    REF(ior);
    REF(transmission);

  private:
    const Texture* m_texture;

    Color m_albedo;

    FloatType m_roughness;

    FloatType m_clearcoat;
    FloatType m_clearcoat_roughness;

    FloatType m_ior;
    FloatType m_transmission;
  };

} // namespace Oxy
