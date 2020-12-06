#pragma once

#include "renderer/primitives/base.hpp"
#include "renderer/utils.hpp"

namespace Oxy::Primitive::Traits {

  template <PrimitiveType T>
  BoundingBox bbox(const PrimitiveImpl<T>& primitive) = delete;

  template <PrimitiveType T>
  Vec3 midpoint(const PrimitiveImpl<T>& primitive) = delete;

  template <PrimitiveType T>
  IntersectResult intersect_ray(const PrimitiveImpl<T>& primitive, const SingleRay& ray) = delete;

  template <PrimitiveType T>
  Vec3 hitnormal(const PrimitiveImpl<T>& primitive, const Vec3& point) = delete;

} // namespace Oxy::Primitive::Traits
