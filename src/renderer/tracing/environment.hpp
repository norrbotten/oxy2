#pragma once

#include "renderer/material/color.hpp"
#include "renderer/types.hpp"

namespace Oxy::Tracing {

  class EnvironmentMap {
  public:
    virtual ~EnvironmentMap() {}
    virtual Color sample(const SingleRay& ray) const = 0;
  };

} // namespace Oxy::Tracing
