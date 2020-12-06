#pragma once

#include "renderer/primitives/sphere.hpp"
#include "renderer/tracing/object.hpp"

namespace Oxy {

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

        ctx.object = this;
      }

      return ctx;
    };

  private:
    Primitive::SpherePrimitive m_sphere;
  };

} // namespace Oxy