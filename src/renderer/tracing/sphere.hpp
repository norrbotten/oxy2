#pragma once

#include "renderer/primitives/sphere.hpp"
#include "renderer/tracing/object.hpp"

namespace Oxy::Tracing {

  /*
    Sphere implementation
  */
  class TracableSphere final : public TracableObject {
  public:
    TracableSphere(const Vec3& center, FloatType radius)
        : m_sphere(center, radius) {}

    virtual IntersectionContext intersect_ray(const SingleRay& ray) override {
      IntersectionContext ctx;
      ctx.ray = ray;

      if (auto isect = Primitive::Traits::intersect_ray(m_sphere, ray); isect.has_value()) {
        ctx.hit = true;
        ctx.t   = isect.value();

        ctx.hitpos    = ray.origin + ray.direction * isect.value();
        ctx.hitnormal = Primitive::Traits::hitnormal(m_sphere, ctx.hitpos);

        auto& n = ctx.hitnormal;
        ctx.uv  = Vec2(n.y * 0.5 + 0.5, 4 * std::atan2(n.x, n.z) / (2 * M_PI) + 0.5);

        ctx.object = this;
      }

      return ctx;
    };

    virtual Vec3 random_point_on_surface() const override {
      auto vec = random_vector_on_unit_sphere();
      return m_sphere.center() + vec * m_sphere.radius();
    }

    virtual SingleRay random_ray_from_surface() const override {
      auto vec   = random_vector_on_unit_sphere();
      auto point = m_sphere.center() + vec * (m_sphere.radius() + 1e-6);

      return SingleRay{point, random_vector_on_hemisphere(vec)};
    }

  private:
    Primitive::SpherePrimitive m_sphere;
  };

} // namespace Oxy::Tracing
