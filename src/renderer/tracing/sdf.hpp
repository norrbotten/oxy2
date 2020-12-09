#pragma once

#include <functional>

#include "renderer/tracing/object.hpp"

namespace Oxy {

  using SDF = FloatType (*)(const Vec3&);

  /*
    SDF (signed distance field) implementation
  */
  class TracableSDF final : public TracableObject {
  public:
    /*
      max_steps is the upper limit of sphere tracing steps

      TODO:
      implement origin and scale
    */
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

      // do sphere tracing to find the intersection distance
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

  /*
    Basic SDF shapes
  */
  namespace SDFShape {
    inline FloatType sphere(const Vec3& p, FloatType radius) { return glm::length(p) - radius; }

    inline FloatType box(const Vec3& p, const Vec3& size) {
      auto q = glm::abs(p) - size;
      return glm::length(glm::max(q, Vec3(0))) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0);
    }

    inline FloatType rounded_box(const Vec3& p, const Vec3& size, FloatType radius) {
      auto q = glm::abs(p) - size;
      return glm::length(glm::max(q, Vec3(0))) + glm::min(glm::max(q.x, q.z), 0.0) - radius;
    }

    inline FloatType torus(const Vec3& p, const Vec2& params) {
      auto q = Vec2(glm::length(Vec2(p.y, p.z)) - params.x, p.x);
      return glm::length(q) - params.y;
    }

    inline FloatType capped_cylinder(const Vec3& p, const Vec3& a, const Vec3& b, FloatType r) {
      auto      ba   = b - a;
      auto      pa   = p - a;
      FloatType baba = glm::dot(ba, ba);
      FloatType paba = glm::dot(pa, ba);
      FloatType x    = glm::length(pa * baba - ba * paba) - r * baba;
      FloatType y    = glm::abs(paba - baba * 0.5) - baba * 0.5;
      FloatType x2   = x * x;
      FloatType y2   = y * y * baba;
      FloatType d    = (glm::max(x, y) < 0.0) ? -glm::min(x2, y2)
                                           : (((x > 0.0) ? x2 : 0.0) + ((y > 0.0) ? y2 : 0.0));

      return glm::sign(d) * glm::sqrt(glm::abs(d)) / baba;
    }

    inline FloatType capsule(const Vec3& p, const Vec3& a, const Vec3& b, FloatType r) {
      auto pa = p - a;
      auto ba = b - a;
      auto h  = glm::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), 0.0, 1.0);
      return glm::length(pa - ba * h) - r;
    }

  } // namespace SDFShape

  /*
    SDF alterations
  */
  namespace SDFAlt {
    inline FloatType round(FloatType in, FloatType radius) { return in - radius; }

    inline FloatType onion(FloatType in, FloatType thiccness) { return glm::abs(in) - thiccness; }

    inline FloatType metric(const Vec3& in, FloatType power) {
      auto p = Vec3(glm::pow(in.x, power), glm::pow(in.y, power), glm::pow(in.z, power));
      return glm::pow(p.x + p.y + p.z, 1.0 / power);
    }

  } // namespace SDFAlt

  /*
    SDF operations
  */
  namespace SDFOp {
    // Adds two shapes (union operation)
    inline FloatType add(FloatType a, FloatType b) { return glm::min(a, b); }

    // Subtracts two shapes, cuts A from B
    inline FloatType sub(FloatType a, FloatType b) { return glm::max(-a, b); }

    // Intersection, returns the volume where A and B intersect
    inline FloatType intersect(FloatType a, FloatType b) { return glm::max(a, b); }

    // Same as above but gives smoothed edges
    inline FloatType smooth_add(FloatType a, FloatType b, FloatType k) {
      auto h = glm::clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
      return glm::mix(b, a, h) - k * h * (1.0 - h);
    }

    // Same as above but gives smoothed edges
    inline FloatType smooth_sub(FloatType a, FloatType b, FloatType k) {
      auto h = glm::clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
      return glm::mix(b, -a, h) + k * h * (1.0 - h);
    }

    // Same as above but gives smoothed edges
    inline FloatType smooth_intersect(FloatType a, FloatType b, FloatType k) {
      auto h = glm::clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
      return glm::mix(b, a, h) + k * h * (1.0 - h);
    }

  } // namespace SDFOp

  /*
    SDF positioning
    All these take an SDF function as an argument, so to do
    multiple wrap the inner in a lambda
  */
  namespace SDFPos {

    /*
      Applies an arbitrary transform to the SDF
    */
    template <typename SDF, typename... SDFArgs>
    FloatType transform(const Vec3& p, const Mat4& transform, SDF primitive, SDFArgs... args) {
      return primitive(Vec3(glm::inverse(transform) * Vec4(p, 1.0)), args...);
    }

    /*
      Offsets the SDF in space
    */
    template <typename SDF, typename... SDFArgs>
    FloatType offset(const Vec3& p, const Vec3& offset, SDF primitive, SDFArgs... args) {
      return primitive(p - offset, args...);
    }

    /*
      Scales the SDF
    */
    template <typename SDF, typename... SDFArgs>
    FloatType scale(const Vec3& p, FloatType scale, SDF primitive, SDFArgs... args) {
      return primitive(p / scale, args...) * scale;
    }

    /*
      Infinitely repeats the SDF in space
    */
    template <typename SDF, typename... SDFArgs>
    FloatType repeat(const Vec3& p, const Vec3& repeat, SDF primitive, SDFArgs... args) {
      auto q = glm::mod(p + 0.5 * repeat, repeat) - 0.5 * repeat;
      return primitive(q, args...);
    }

    /*
      Repeats the SDF in the space bounded by a box
    */
    template <typename SDF, typename... SDFArgs>
    FloatType repeat_finite(const Vec3& p, const Vec3& repeat, const Vec3& bound, SDF primitive,
                            SDFArgs... args) {
      auto q = p - repeat * glm::clamp(glm::round(p / repeat), -bound, bound);
      return primitive(q, args...);
    }

  } // namespace SDFPos

} // namespace Oxy