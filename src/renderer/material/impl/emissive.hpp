#pragma once

#include <filesystem>

#include "renderer/material/material.hpp"
#include "renderer/material/texture.hpp"

#include "renderer/types.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  class Emissive final : public Material {
  public:
    Emissive(Color energy)
        : m_albedo(energy) {}

    virtual Color sample(const IntersectionContext& ctx) const override {
      (void)ctx;
      return m_albedo;
    }

    virtual LightRay scatter(const IntersectionContext& ctx) const override {
      (void)ctx;

      LightRay result;

      result.light         = this->sample(ctx);
      result.ray.origin    = ctx.hitpos + ctx.hitnormal * 1e-6;
      result.ray.direction = random_vector_on_hemisphere(ctx.hitnormal);

      return result;
    }

    virtual bool is_emissive() const override { return true; }

    virtual Color get_emission_color() const override { return m_albedo; }

  private:
    Color m_albedo;
  };

} // namespace Oxy