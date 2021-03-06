#pragma once

#include <memory>
#include <numeric>
#include <vector>

#include "renderer/sdl/execution.hpp"

#include "renderer/tracing/environment.hpp"
#include "renderer/tracing/object.hpp"

namespace Oxy::Tracing {

  /*
    World is a class which holds pointers to all of the objects in the scene

    World takes ownership of its objects and cleans up their allocated memory on
    destruction, this means you cant add multiple of the same object instance, since
    then it would double free it.
  */
  class World {
  public:
    ~World() {
      if (m_environment)
        delete m_environment;
    }

    IntersectionContext intersect_ray(const SingleRay& ray) const {
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

    IntersectionContext intersect_line(const Vec3& start, const Vec3& end) const {
      IntersectionContext res;

      auto len = glm::length(end - start);
      auto ray = SingleRay{start, (end - start) / len};

      auto min = len + 1e-6;
      for (auto&& obj : m_objects) {
        auto obj_ctx = obj->intersect_ray(ray);

        if (obj_ctx.hit && obj_ctx.t < min) {
          res = obj_ctx;
          min = res.t;
        }
      }

      return res;
    }

    Color sample_environment(const SingleRay& ray) const {
      if (m_environment != nullptr)
        return m_environment->sample(ray);

      return Color();
    }

    template <typename T>
    void add_object(T* obj) {
      m_objects.push_back(std::unique_ptr<T>(obj));
    }

    auto& objects() { return m_objects; }

    void set_environment(EnvironmentMap* env) { m_environment = env; }

  private:
    std::vector<std::unique_ptr<TracableObject>> m_objects;
    EnvironmentMap*                              m_environment{nullptr};
  };

} // namespace Oxy::Tracing
