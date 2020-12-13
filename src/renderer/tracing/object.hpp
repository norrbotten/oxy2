#pragma once

#include <memory>

#include "renderer/material/material.hpp"
#include "renderer/utils.hpp"

namespace Oxy::Tracing {

  /*
    Abstract tracable class
    Tracables have a material and only need to implement ray intersection which
    returns an intersection context
  */
  class TracableObject {
  public:
    virtual IntersectionContext intersect_ray(const SingleRay& ray) = 0;

    REF(material);

  private:
    std::shared_ptr<Material> m_material;
  };

} // namespace Oxy::Tracing
