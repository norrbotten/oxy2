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

    Color sample(const IntersectionContext& ctx) override {
      return Color(); // emissives shouldnt be sampled, but still need this
    }

    LightRay scatter(const IntersectionContext& ctx) override {
      LightRay result;

      return result;
    }

  private:
    Color m_albedo;
  };

} // namespace Oxy
