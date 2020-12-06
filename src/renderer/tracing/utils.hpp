#pragma once

#include <random>

#include "renderer/types.hpp"

namespace Oxy {

  struct IntersectionContext {
    SingleRay ray;

    bool   hit = false;
    double t   = 0;

    Vec2 uv{0, 0};

    Vec3 hitpos;
    Vec3 hitnormal;
  };

  // https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c
  // Author: Galik
  template <typename Numeric, typename Generator = std::mt19937>
  Numeric random(Numeric from, Numeric to) {
    thread_local static Generator gen(std::random_device{}());

    using dist_type = typename std::conditional<std::is_integral<Numeric>::value,
                                                std::uniform_int_distribution<Numeric>,
                                                std::uniform_real_distribution<Numeric>>::type;

    thread_local static dist_type dist;

    return dist(gen, typename dist_type::param_type{from, to});
  }

  template <typename Vector>
  Vector reflect(const Vector& incident, const Vector& normal) {
    return incident - 2 * glm::dot(incident, normal) * normal;
  }

  template <typename Vector>
  Vector refract(const Vector& incident, const Vector& normal, FloatType ior) {
    auto n = 1.0 / ior;

    auto cos = -glm::dot(incident, normal);
    auto sin = n * n * (1.0 - cos * cos);

    if (sin > 1.0)
      return reflect(incident, -normal); // total internal reflection

    auto cos2 = glm::sqrt(1.0 - sin);
    return n * incident + (n * cos * cos2) * normal;
  }

  Vec3 random_vector_on_unit_sphere() {
    auto theta  = random<FloatType>(0, 2 * M_PI);
    auto phi    = glm::acos(1 - 2 * random<FloatType>(0, 1));
    auto sinphi = glm::sin(phi);
    return Vec3{
        sinphi * glm::cos(theta),
        sinphi * glm::sin(theta),
        glm::cos(phi),
    };
  }

  Vec3 random_vector_on_hemisphere(const Vec3& normal) {
    auto vec = random_vector_on_unit_sphere();

    if (glm::dot(vec, normal) < 0)
      return -vec;

    return vec;
  }

  Vec3 random_vector_on_cone(const Vec3& normal, FloatType theta) {
    // from the ipython notebook in trash/
    // there werent any real method in coming up with this,
    // but it seems to work and i THINK it gives a uniform dist

    auto z    = glm::cos(2 * theta);
    auto phi  = random<FloatType>(0, 2 * M_PI);
    auto sqrt = glm::sqrt(1.0 - z * z);
    auto x    = sqrt * glm::cos(phi);
    auto y    = sqrt * glm::sin(phi);

    Vec3 rotaxis{glm::sin(phi), -glm::cos(phi), 0};
    auto angle = 2 * theta * (1 - 2 * glm::acos(random<FloatType>(0, 1)) / M_PI);

    // auto rotmatrix = glm::rotate(rotaxis, angle, Vec3{1, 0, 0});
    auto rotmatrix = glm::mat4_cast(glm::angleAxis(angle, rotaxis));
    auto point4    = rotmatrix * Vec4(x, y, z, 1);

    if (normal.z > 0.999999)
      return Vec3(point4);

    if (normal.z < -0.999999)
      return -Vec3(point4);

    auto rotaxis2 = glm::normalize(glm::cross(normal, Vec3(0, 0, -1)));
    // auto rotmatrix2 = glm::rotate(Vec4(rotaxis2, 1), glm::acos(normal.z), Vec3{1, 0, 0});
    auto rotmatrix2 = glm::mat4_cast(glm::angleAxis(glm::acos(normal.z), rotaxis2));

    return Vec3(rotmatrix2 * point4);
  }

} // namespace Oxy
