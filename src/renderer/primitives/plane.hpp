#pragma once

#include <cmath>

#include "renderer/primitives/base.hpp"
#include "renderer/primitives/traits.hpp"

namespace Oxy::Primitive {

  /*
    Plane primitive implementation
  */
  template <>
  class PrimitiveImpl<PrimitiveType::Plane> {
  public:
    PrimitiveImpl(const Vec3& point, const Vec3& normal)
        : m_point(point)
        , m_normal(normal) {}

    CREF(point);
    CREF(normal);

  private:
    Vec3 m_point, m_normal;
  };

  using PlanePrimitive = PrimitiveImpl<PrimitiveType::Plane>;

  namespace Traits {

    template <>
    inline BoundingBox bbox(const PlanePrimitive& sphere) {
      (void)sphere;
      return {Vec3(std::numeric_limits<FloatType>::lowest()),
              Vec3(std::numeric_limits<FloatType>::max())};
    }

    template <>
    inline Vec3 midpoint(const PlanePrimitive& sphere) {
      return sphere.point();
    }

    template <>
    inline IntersectResult intersect_ray(const PlanePrimitive& primitive, const SingleRay& ray) {
      auto a = glm::dot(primitive.normal(), ray.direction);

      if (glm::abs(a) > 1e-6) {
        auto t = glm::dot(primitive.point() - ray.origin, primitive.normal()) / a;
        if (t > 0)
          return t;
      }

      return {};
    }

    template <>
    inline Vec3 hitnormal(const PlanePrimitive& primitive, const Vec3& point) {
      (void)point;
      return primitive.point();
    }

  } // namespace Traits

} // namespace Oxy::Primitive
