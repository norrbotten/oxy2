#pragma once

#include "renderer/primitives/base.hpp"
#include "renderer/utils.hpp"

namespace Oxy::Primitive::Traits {

  /*
    Returns the bounding box (min, max) of a primitive
  */
  template <PrimitiveType T>
  BoundingBox bbox(const PrimitiveImpl<T>& primitive) = delete;

  /*
    Returns the geometric midpoint of a primitive
  */
  template <PrimitiveType T>
  Vec3 midpoint(const PrimitiveImpl<T>& primitive) = delete;

  /*
    Intersects a ray with the primitive
  */
  template <PrimitiveType T>
  IntersectResult intersect_ray(const PrimitiveImpl<T>& primitive, const SingleRay& ray) = delete;

  /*
    Returns the primitives hitnormal at a certain point
    Only expect normalized vectors from this if the point is actually on the primitives surface
  */
  template <PrimitiveType T>
  Vec3 hitnormal(const PrimitiveImpl<T>& primitive, const Vec3& point) = delete;

} // namespace Oxy::Primitive::Traits
