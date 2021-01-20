#pragma once

#include <memory>

#include "renderer/material/bsdf.hpp"
#include "renderer/utils.hpp"

#include "renderer/pools/material_pool.hpp"

namespace Oxy::Tracing {

  /*
    Abstract tracable class
    Tracables have a material and need to implement ray intersection and a method returning a random
    point on its surface
  */
  class TracableObject {
  public:
    virtual IntersectionContext intersect_ray(const SingleRay& ray) = 0;

    virtual Vec3      random_point_on_surface() const = 0;
    virtual SingleRay random_ray_from_surface() const = 0;

    REF(material);

  private:
    Pools::MaterialRef m_material;
  };

} // namespace Oxy::Tracing
