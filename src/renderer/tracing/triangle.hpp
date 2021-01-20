#pragma once

#include "renderer/primitives/triangle.hpp"
#include "renderer/tracing/object.hpp"

namespace Oxy::Tracing {

  /*
    Triangle implementation
  */
  class TracableTriangle final : public TracableObject {
  public:
    TracableTriangle(const Vec3& p0, const Vec3& p1, const Vec3& p2)
        : m_triangle(p0, p1, p2) {}

    virtual IntersectionContext intersect_ray(const SingleRay& ray) override {
      IntersectionContext ctx;
      ctx.ray = ray;

      if (auto isect = Primitive::Traits::intersect_ray(m_triangle, ray); isect.has_value()) {
        ctx.hit = true;
        ctx.t   = isect.value();

        ctx.hitpos    = ray.origin + ray.direction * isect.value();
        ctx.hitnormal = Primitive::Traits::hitnormal(m_triangle, ctx.hitpos);

        if (glm::dot(ctx.hitnormal, ray.direction) > 0)
          ctx.hitnormal *= -1;

        ctx.uv = {ctx.hitpos.x, ctx.hitpos.y};

        ctx.object = this;
      }

      return ctx;
    };

    virtual Vec3 random_point_on_surface() const override {
      auto a = random<FloatType>(0, 1);
      auto b = random<FloatType>(0, 1);

      if (a + b >= 1.0) {
        a = 1.0 - a;
        b = 1.0 - b;
      }

      return m_triangle.p0() + a * (m_triangle.p1() - m_triangle.p0()) +
             b * (m_triangle.p2() - m_triangle.p0());
    }

    virtual SingleRay random_ray_from_surface() const override {
      auto point = this->random_point_on_surface();
      return SingleRay{point, random_vector_on_hemisphere(m_triangle.normal())};
    }

  private:
    Primitive::TrianglePrimitive m_triangle;
  };

} // namespace Oxy::Tracing
