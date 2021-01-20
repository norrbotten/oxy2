#pragma once

#include <cassert>

#include "renderer/primitives/plane.hpp"
#include "renderer/tracing/object.hpp"

namespace Oxy::Tracing {

  /*
    Plane implementation
  */
  class TracablePlane final : public TracableObject {
  public:
    TracablePlane(const Vec3& point, const Vec3& normal)
        : m_plane(point, normal) {

      if (glm::abs(normal.z) > (1.0 - 1e-6))
        m_plane_tangent = Vec3(0.0, 1.0, 0.0);
      else
        m_plane_tangent = glm::normalize(glm::cross(normal, Vec3(0.0, 0.0, 1.0)));

      m_plane_bitangent = glm::cross(m_plane_tangent, normal);
    }

    virtual IntersectionContext intersect_ray(const SingleRay& ray) override {
      IntersectionContext ctx;
      ctx.ray = ray;

      if (auto isect = Primitive::Traits::intersect_ray(m_plane, ray); isect.has_value()) {
        ctx.hit = true;
        ctx.t   = isect.value();

        ctx.hitpos    = ray.origin + ray.direction * isect.value();
        ctx.hitnormal = Primitive::Traits::hitnormal(m_plane, ctx.hitpos);

        if (glm::dot(ctx.hitnormal, ray.direction) > 0)
          ctx.hitnormal *= -1;

        auto diff = ctx.hitpos - m_plane.point();

        ctx.uv = Vec2(glm::mod(glm::dot(diff, m_plane_tangent), 1.0),
                      glm::mod(glm::dot(diff, m_plane_bitangent), 1.0));

        ctx.object = this;
      }

      return ctx;
    };

    virtual Vec3 random_point_on_surface() const override {
      assert(false); // planes are infinitely large, cant really get a random point
    }

    virtual SingleRay random_ray_from_surface() const override {
      assert(false); // planes are infinitely large, cant really get a random point
    }

  private:
    Primitive::PlanePrimitive m_plane;

    // used for calculating uv's
    Vec3 m_plane_tangent;
    Vec3 m_plane_bitangent;
  };

} // namespace Oxy::Tracing
