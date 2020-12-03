#pragma once

#include <cmath>

#include "renderer/primitives/base.hpp"
#include "renderer/primitives/traits.hpp"

namespace Oxy::Primitive {

  template <>
  class PrimitiveImpl<PrimitiveType::Sphere> {
  public:
    PrimitiveImpl(const Vec3& center, const FloatType& radius)
        : m_center(center)
        , m_radius(radius)
        , m_radius2(radius * radius) {}

    const auto& center() const { return m_center; }
    const auto& radius() const { return m_radius; }
    const auto& radius2() const { return m_radius2; }

  private:
    Vec3      m_center;
    FloatType m_radius, m_radius2;
  };

  using SpherePrimitive = PrimitiveImpl<PrimitiveType::Sphere>;

  namespace Traits {

    template <>
    BoundingBox bbox(const SpherePrimitive& sphere) {
      return BoundingBox{sphere.center() - sphere.radius(), sphere.center() + sphere.radius()};
    }

    template <>
    Vec3 midpoint(const SpherePrimitive& sphere) {
      return sphere.center();
    }

    template <>
    IntersectResult intersect_ray(const SpherePrimitive& primitive, const SingleRay& ray) {
      Vec3 oc = ray.origin - primitive.center();

      FloatType a = glm::dot(ray.direction, ray.direction);
      FloatType b = 2.0 * glm::dot(oc, ray.direction);
      FloatType c = glm::dot(oc, oc) - primitive.radius2();

      FloatType disc = b * b - 4 * a * c;

      if (disc < 0)
        return {};

      FloatType t = (-b - std::sqrt(disc)) / (2.0 * a);

      if (t < 0)
        return {};

      return t;
    }

  } // namespace Traits

} // namespace Oxy::Primitive
