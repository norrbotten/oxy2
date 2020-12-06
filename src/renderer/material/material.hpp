#pragma once

#include "renderer/material/color.hpp"
#include "renderer/tracing/utils.hpp"

namespace Oxy {

  struct LightRay {
    SingleRay ray;
    Color     light;
  };

  class Material {
  public:
    virtual Color    sample(const IntersectionContext& ctx)  = 0;
    virtual LightRay scatter(const IntersectionContext& ctx) = 0;

    virtual bool     is_emissive() { return false; }
    virtual LightRay get_emission() { return {}; }
  };

} // namespace Oxy