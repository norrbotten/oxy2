#pragma once

#include "renderer/accel/bvh.hpp"

#include "renderer/primitives/triangle.hpp"
#include "renderer/tracing/object.hpp"

namespace Oxy::Tracing {

  class TracableMesh final : public TracableObject {
  public:
    TracableMesh(const std::vector<Primitive::TrianglePrimitive>& tris)
        : m_triangles(tris)
        , m_bvh(Accel::build_bvh(m_triangles, m_packed_triangles, 0, m_triangles.size())) {}

    virtual IntersectionContext intersect_ray(const SingleRay& ray) override {
      IntersectionContext ctx;

      FloatType dummy;
      if (!Accel::ray_vs_aabb(ray.origin, ray.direction, m_bvh->bbox.min, m_bvh->bbox.max, dummy))
        return ctx;

      if (auto bvh_res = Accel::traverse_bvh(m_bvh, ray); bvh_res.hit) {
        auto& tri = m_triangles.at(bvh_res.index);

        ctx.hit = true;
        ctx.t   = bvh_res.t;

        ctx.hitpos    = ray.origin + ray.direction * bvh_res.t;
        ctx.hitnormal = tri.normal();

        if (glm::dot(ctx.hitnormal, ray.direction) > 0)
          ctx.hitnormal *= -1;

        ctx.uv = {ctx.hitpos.x, ctx.hitpos.y};

        ctx.object = this;
      }

      return ctx;
    }

    virtual Vec3 random_point_on_surface() const override {
      // not sure if this is uniformly distributed, but works
      // also the random triangle would need to be picked weighed by its surface area

      // pick a random triangle
      auto tri_n = random<int>(0, m_triangles.size() - 1);
      auto tri   = m_triangles[tri_n];

      auto a = random<FloatType>(0, 1);
      auto b = random<FloatType>(0, 1);

      if (a + b >= 1.0) {
        a = 1.0 - a;
        b = 1.0 - b;
      }

      return tri.p0() + a * (tri.p1() - tri.p0()) + b * (tri.p2() - tri.p0());
    }

    virtual SingleRay random_ray_from_surface() const override {
      // pick a random triangle
      auto tri_n = random<int>(0, m_triangles.size() - 1);
      auto tri   = m_triangles[tri_n];

      auto a = random<FloatType>(0, 1);
      auto b = random<FloatType>(0, 1);

      if (a + b >= 1.0) {
        a = 1.0 - a;
        b = 1.0 - b;
      }

      auto point = tri.p0() + a * (tri.p1() - tri.p0()) + b * (tri.p2() - tri.p0());

      return SingleRay{point, random_vector_on_hemisphere(tri.normal())};
    }

  private:
    std::vector<Primitive::TrianglePrimitive> m_triangles;
    std::vector<Accel::AVX2PackedTriangles>   m_packed_triangles;

    Accel::BVHNode<Primitive::TrianglePrimitive>* m_bvh;
  };

} // namespace Oxy::Tracing
