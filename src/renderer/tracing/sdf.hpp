#pragma once

#include <functional>

#include "renderer/tracing/object.hpp"

namespace Oxy {

  using SDF = FloatType (*)(const Vec3&);

  class TracableSDF final : public TracableObject {
  public:
    TracableSDF(const Vec3& origin, FloatType scale, SDF sdf, int max_steps = 256)
        : m_origin(origin)
        , m_scale(scale)
        , m_sdf(sdf)
        , m_max_steps(max_steps) {}

    virtual IntersectionContext intersect_ray(const SingleRay& ray) override {
      static const auto eps = 1e-3;
      static const auto v1  = Vec3(1, -1, -1);
      static const auto v2  = Vec3(-1, -1, 1);
      static const auto v3  = Vec3(-1, 1, -1);
      static const auto v4  = Vec3(1, 1, 1);

      auto normal = [](SDF sdf, const Vec3& point) {
        auto p1 = v1 * sdf(point + v1 * eps);
        auto p2 = v2 * sdf(point + v2 * eps);
        auto p3 = v3 * sdf(point + v3 * eps);
        auto p4 = v4 * sdf(point + v4 * eps);
        return glm::normalize(p1 + p2 + p3 + p4);
      };

      IntersectionContext ctx;
      ctx.ray = ray;

      auto t = ([this, ray] {
        FloatType depth = 0;

        for (int i = 0; i < m_max_steps; i++) {
          auto dist = m_sdf(ray.origin + ray.direction * depth);

          if (dist < 1e-6)
            return depth;

          depth += dist;

          if (depth > 65536)
            return -1.0;
        }

        return -1.0;
      })();

      if (t >= 0) {
        ctx.hit = true;
        ctx.t   = t;

        ctx.hitpos    = ray.origin + ray.direction * t;
        ctx.hitnormal = normal(m_sdf, ctx.hitpos);

        ctx.object = this;
      }

      return ctx;
    }

  private:
    Vec3      m_origin;
    FloatType m_scale;
    SDF       m_sdf;

    int m_max_steps;
  };

  // from https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

  namespace SDFShape {
    inline FloatType sphere(const Vec3& p, FloatType radius) { return glm::length(p) - radius; }

    inline FloatType box(const Vec3& p, const Vec3& size) {
      auto q = glm::abs(p) - size;
      return glm::length(glm::max(q, Vec3(0))) + glm::min(glm::max(q.x, q.z), 0.0);
    }

    inline FloatType rounded_box(const Vec3& p, const Vec3& size, FloatType radius) {
      auto q = glm::abs(p) - size;
      return glm::length(glm::max(q, Vec3(0))) + glm::min(glm::max(q.x, q.z), 0.0) - radius;
    }

    inline FloatType torus(const Vec3& p, const Vec2& params) {
      auto q = Vec2(glm::length(Vec2(p.x, p.z) - params.x), p.y);
      return glm::length(q) - params.y;
    }

  } // namespace SDFShape

  namespace SDFAlt {
    inline FloatType round(FloatType in, FloatType radius) { return in - radius; }

    inline FloatType onion(FloatType in, FloatType thiccness) { return glm::abs(in) - thiccness; }

    inline FloatType metric(const Vec3& in, FloatType power) {
      auto p = Vec3(glm::pow(in.x, power), glm::pow(in.y, power), glm::pow(in.z, power));
      return glm::pow(p.x + p.y + p.z, 1.0 / power);
    }

  } // namespace SDFAlt

  namespace SDFOp {
    inline FloatType add(FloatType a, FloatType b) { return glm::min(a, b); }

    inline FloatType sub(FloatType a, FloatType b) { return glm::max(-a, b); }

    inline FloatType intersect(FloatType a, FloatType b) { return glm::max(a, b); }

    inline FloatType smooth_add(FloatType a, FloatType b, FloatType k) {
      auto h = glm::clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
      return glm::mix(b, a, h) - k * h * (1.0 - h);
    }

    inline FloatType smooth_sub(FloatType a, FloatType b, FloatType k) {
      auto h = glm::clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
      return glm::mix(b, -a, h) + k * h * (1.0 - h);
    }

    inline FloatType smooth_intersect(FloatType a, FloatType b, FloatType k) {
      auto h = glm::clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
      return glm::mix(b, a, h) + k * h * (1.0 - h);
    }

  } // namespace SDFOp

  namespace SDFPos {

    template <typename SDF>
    FloatType offset(const Vec3& p, const Vec3& offset, SDF primitive) {
      return primitive(p - offset);
    }

    template <typename SDF>
    FloatType scale(const Vec3& p, FloatType scale, SDF primitive) {
      return primitive(p / scale) * scale;
    }

    template <typename SDF>
    FloatType repeat(const Vec3& p, const Vec3& repeat, SDF primitive) {
      auto q = glm::mod(p + 0.5 * repeat, repeat) - 0.5 * repeat;
      return primitive(q);
    }

    template <typename SDF, typename... SDFArgs>
    FloatType repeat_finite(const Vec3& p, const Vec3& repeat, const Vec3& bound, SDF primitive,
                            SDFArgs... args) {
      auto q = p - repeat * glm::clamp(glm::round(p / repeat), -bound, bound);
      return primitive(q, args...);
    }

  } // namespace SDFPos

} // namespace Oxy