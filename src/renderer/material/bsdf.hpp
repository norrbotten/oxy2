#pragma once

#include "renderer/material/texture.hpp"

#include "renderer/pools/texture_pool.hpp"

#include "renderer/types.hpp"
#include "renderer/utils.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  struct LightRay {
    SingleRay ray;
    Color     energy;
    Color     absorption;
  };

  class BSDF final {
  public:
    BSDF() {}

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

    Color sample(const IntersectionContext& ctx) const {
      return m_texture != nullptr ? m_texture->sample(ctx.uv.x, ctx.uv.y) * m_albedo : m_albedo;
    }

    LightRay scatter(const IntersectionContext& ctx) const {
      // TODO: glossy refraction
      // TODO: proper glossy reflection (no more mirror reflect + random)

      LightRay result;

      result.energy     = m_emission_energy;
      result.absorption = this->sample(ctx);

      if (random<FloatType>(0, 1) < m_transmission) {
        auto entering_solid = glm::dot(ctx.hitnormal, ctx.ray.direction) < 0;
        auto ior1           = entering_solid ? 1.0 : m_ior;
        auto ior2           = entering_solid ? m_ior : 1.0;

        auto reflection = schlick(ctx.ray.direction, ctx.hitnormal, ior1 / ior2);

        auto refracted = random<FloatType>(0, 1) < reflection
                             ? reflect(ctx.ray.direction, ctx.hitnormal)
                             : refract(ctx.ray.direction, ctx.hitnormal, ior1, ior2);

        result.ray = SingleRay{ctx.hitpos + refracted * 1e-6, refracted};
      }
      else if (random<FloatType>(0, 1) < m_clearcoat) {
        Vec3 reflected;

        if (m_clearcoat_roughness == 0.0)
          reflected = reflect(ctx.ray.direction, ctx.hitnormal);
        else if (m_clearcoat_roughness == 1.0)
          reflected = random_vector_on_hemisphere(ctx.hitnormal);
        else
          reflected = glm::normalize(reflect(ctx.ray.direction, ctx.hitnormal) +
                                     random_vector_on_unit_sphere() * m_clearcoat_roughness);

        result.ray = SingleRay{ctx.hitpos + ctx.hitnormal * 1e-6, reflected};
      }
      else {
        Vec3 reflected;

        if (m_roughness == 0.0)
          reflected = reflect(ctx.ray.direction, ctx.hitnormal);
        else if (m_roughness == 1.0)
          reflected = random_vector_on_hemisphere(ctx.hitnormal);
        else
          reflected = glm::normalize(reflect(ctx.ray.direction, ctx.hitnormal) +
                                     random_vector_on_unit_sphere() * m_roughness);

        result.ray = SingleRay{ctx.hitpos + ctx.hitnormal * 1e-6, reflected};
      }

      return result;
    }

    FloatType pdf(const Vec3& incident, const Vec3& hitnormal, const Vec3& out) const {
      // is this even called pdf? (as in probability distribution function)
      // it just returns the probability of the incident ray reflecting of the
      // hitnormal in the direction of out

      auto incident_reflect = reflect(incident, hitnormal);
      auto dot              = glm::abs(glm::dot(incident_reflect, out));

      FloatType res = 0.0;

      // emitted light (should do luminousity here instead)
      res += (m_emission_energy.r() + m_emission_energy.g() + m_emission_energy.b()) / 3.0;

      // diffuse bounce
      res += dot * m_roughness;

      // diffuse clearcoat bounce
      res += dot * m_clearcoat * m_clearcoat_roughness;

      return res;
    }

    REF(albedo);
    REF(roughness);
    REF(clearcoat);
    REF(clearcoat_roughness);
    REF(ior);
    REF(transmission);
    REF(emission_energy);

  private:
    const Texture* m_texture;

    Color m_albedo;

    FloatType m_roughness;

    FloatType m_clearcoat;
    FloatType m_clearcoat_roughness;

    FloatType m_ior;
    FloatType m_transmission;

    Color m_emission_energy;
  };

} // namespace Oxy
