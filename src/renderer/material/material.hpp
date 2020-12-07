#pragma once

#include "renderer/material/color.hpp"
#include "renderer/utils.hpp"

namespace Oxy {

  struct LightRay {
    SingleRay ray;
    Color     light;
  };

  class Material {
  public:
    virtual Color    sample(const IntersectionContext& ctx) const  = 0;
    virtual LightRay scatter(const IntersectionContext& ctx) const = 0;

    virtual bool is_emissive() const { return false; }
  };

} // namespace Oxy
