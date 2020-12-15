#pragma once

#include <cmath>

#include "renderer/primitives/base.hpp"
#include "renderer/primitives/traits.hpp"

#include "renderer/material/color.hpp"

namespace Oxy::Primitive {

  /*
    Plane primitive implementation
  */
  template <>
  class PrimitiveImpl<PrimitiveType::Triangle> {
  public:
    PrimitiveImpl(const Vec3& p0, const Vec3& p1, const Vec3& p2)
        : m_p0(p0)
        , m_p1(p1)
        , m_p2(p2) {
      compute_normal();
      compute_bbox();
    }

    CREF(p0);
    CREF(p1);
    CREF(p2);
    CREF(normal);
    CREF(bbox);

  private:
    void compute_normal() { m_normal = glm::normalize(glm::cross(m_p2 - m_p0, m_p1 - m_p0)); }

    void compute_bbox() {
      double min_x = std::min(m_p0.x, std::min(m_p1.x, m_p2.x));
      double min_y = std::min(m_p0.y, std::min(m_p1.y, m_p2.y));
      double min_z = std::min(m_p0.z, std::min(m_p1.z, m_p2.z));

      double max_x = std::max(m_p0.x, std::max(m_p1.x, m_p2.x));
      double max_y = std::max(m_p0.y, std::max(m_p1.y, m_p2.y));
      double max_z = std::max(m_p0.z, std::max(m_p1.z, m_p2.z));

      m_bbox = {Vec3{min_x, min_y, min_z}, Vec3{max_x, max_y, max_z}};
    }

  private:
    Vec3 m_p0, m_p1, m_p2;
    Vec3 m_normal;

    BoundingBox m_bbox;
  };

  using TrianglePrimitive = PrimitiveImpl<PrimitiveType::Triangle>;

  namespace Traits {

    template <>
    inline BoundingBox bbox(const TrianglePrimitive& triangle) {
      return triangle.bbox();
    }

    template <>
    inline Vec3 midpoint(const TrianglePrimitive& triangle) {
      return (triangle.p0() + triangle.p1() + triangle.p2()) / 3.0;
    }

    template <>
    inline IntersectResult intersect_ray(const TrianglePrimitive& triangle, const SingleRay& ray) {
      auto v0v1 = triangle.p1() - triangle.p0();
      auto v0v2 = triangle.p2() - triangle.p0();
      auto pvec = glm::cross(ray.direction, v0v2);

      auto det = glm::dot(v0v1, pvec);

      if (std::fabs(det) < 1e-6)
        return {};

      auto inv_det = 1.0 / det;

      auto tvec = ray.origin - triangle.p0();
      auto qvec = glm::cross(tvec, v0v1);

      auto u = glm::dot(tvec, pvec) * inv_det;
      auto v = glm::dot(ray.direction, qvec) * inv_det;

      if ((u < 0) | (u > 1) | (v < 0) | (u + v > 1))
        return {};

      auto t = glm::dot(v0v2, qvec) * inv_det;

      if (t < 1e-6)
        return {};

      return t;
    }

    template <>
    inline Vec3 hitnormal(const TrianglePrimitive& triangle, const Vec3& point) {
      (void)point;
      return triangle.normal();
    }

  } // namespace Traits

} // namespace Oxy::Primitive
