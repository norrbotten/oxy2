#pragma once

#include "renderer/accel/bvh.hpp"

#include "renderer/primitives/triangle.hpp"
#include "renderer/tracing/object.hpp"

namespace Oxy::Tracing {

  class TracableMesh final : public TracableObject {
  public:
    TracableMesh(const std::vector<Primitive::TrianglePrimitive>& tris)
        : m_triangles(tris)
        , m_bvh(Accel::build_bvh(m_triangles, 0, m_triangles.size())) {}

    virtual IntersectionContext intersect_ray(const SingleRay& ray) override {
      IntersectionContext ctx;

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

  private:
    std::vector<Primitive::TrianglePrimitive>     m_triangles;
    Accel::BVHNode<Primitive::TrianglePrimitive>* m_bvh;
  };

} // namespace Oxy::Tracing
