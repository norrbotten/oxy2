#pragma once

#include "renderer/material/color.hpp"
#include "renderer/tracing/utils.hpp"

namespace Oxy {

  class Material {
  public:
    virtual Color     sample(const IntersectionContext& ctx)  = 0;
    virtual SingleRay reflect(const IntersectionContext& ctx) = 0;
  };

} // namespace Oxy
