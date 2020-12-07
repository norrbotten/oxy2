#pragma once

#include <memory>
#include <numeric>
#include <vector>

#include "renderer/tracing/object.hpp"

namespace Oxy {

  class World {
  public:
    IntersectionContext intersect_ray(const SingleRay& ray) {
      IntersectionContext res;

      auto min = std::numeric_limits<FloatType>::max();
      for (auto&& obj : m_objects) {
        auto obj_ctx = obj->intersect_ray(ray);

        if (obj_ctx.hit && obj_ctx.t < min) {
          res = obj_ctx;
          min = res.t;
        }
      }

      return res;
    }

    IntersectionContext intersect_line(const Vec3& start, const Vec3& end) {
      IntersectionContext res;

      auto len = glm::length(end - start);
      auto ray = SingleRay{start, (end - start) / len};

      auto min = len;
      for (auto&& obj : m_objects) {
        auto obj_ctx = obj->intersect_ray(ray);

        if (obj_ctx.hit && obj_ctx.t < min) {
          res = obj_ctx;
          min = res.t;
        }
      }

      return res;
    }

    template <typename T>
    void add_object(T* obj) {
      m_objects.push_back(std::shared_ptr<T>(obj));
    }

  private:
    std::vector<std::shared_ptr<TracableObject>> m_objects;
  };

} // namespace Oxy