#pragma once

#include <cstring>
#include <fstream>
#include <iomanip>
#include <optional>
#include <random>
#include <sstream>
#include <string>

#include "renderer/types.hpp"

namespace Oxy {

  namespace Tracing {
    class TracableObject;
  }

  /*
    IntersectionContext is returned by tracables,
    any kind of relevant information for coloring samples is stored in it
  */
  struct IntersectionContext {
    SingleRay ray;

    bool   hit = false;
    double t   = 0;

    Vec2 uv{0, 0};

    Vec3 hitpos;
    Vec3 hitnormal;

    Tracing::TracableObject* object = nullptr;
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

  // Returns -1 or 1
  template <typename T>
  T random_sign() {
    return random<float>(0, 1) < 0.5f ? T(-1) : T(1);
  }

  /*
    Reflects a vector around a normal
  */
  template <typename Vector>
  Vector reflect(const Vector& incident, const Vector& normal) {
    return incident - 2 * glm::dot(incident, normal) * normal;
  }

  /*
    Refracts a vector around a normal
  */
  template <typename Vector>
  Vector refract(const Vector& incident, const Vector& normal, FloatType ior1, FloatType ior2) {
    auto n = ior1 / ior2;

    auto cos = -glm::dot(incident, normal);
    auto sin = n * n * (1.0 - cos * cos);

    if (sin > 1.0)
      return reflect(incident, -normal); // total internal reflection

    auto cos2 = glm::sqrt(1.0 - sin);
    return n * incident + (n * cos - cos2) * normal;
  }

  /*
    Returns the probability of a ray reflecting instead of refracting
  */
  template <typename Vector>
  FloatType schlick(const Vector& incident, const Vector& normal, FloatType ior) {
    auto r0 = (1.0 - ior) / (1.0 + ior);
    r0 *= r0;

    auto cos = -glm::dot(incident, normal);
    return r0 + (1.0 - r0) * glm::pow(1.0 - cos, 5);
  }

  /*
    Returns a normalized vector distributed uniformly on a sphere of radius 1
  */
  inline Vec3 random_vector_on_unit_sphere() {
    auto theta  = random<FloatType>(0, 2 * M_PI);
    auto phi    = glm::acos(1 - 2 * random<FloatType>(0, 1));
    auto sinphi = glm::sin(phi);
    return Vec3{
        sinphi * glm::cos(theta),
        sinphi * glm::sin(theta),
        glm::cos(phi),
    };
  }

  /*
    Returns a normalized vector distributed uniformly on a sphere of radius 1,
    with non-random theta/phi
  */
  inline Vec3 random_vector_on_unit_sphere(FloatType arg1, FloatType arg2) {
    auto theta  = arg1 * 2 * M_PI;
    auto phi    = glm::acos(1 - 2 * arg2);
    auto sinphi = glm::sin(phi);
    return Vec3{
        sinphi * glm::cos(theta),
        sinphi * glm::sin(theta),
        glm::cos(phi),
    };
  }

  /*
    Returns a normalized vector distributed uniformly on a hemisphere of radius 1
  */
  inline Vec3 random_vector_on_hemisphere(const Vec3& normal) {
    auto vec = random_vector_on_unit_sphere();

    if (glm::dot(vec, normal) < 0)
      return -vec;

    return vec;
  }

  /*
    Returns a normalized vector distributed uniformly on a hemisphere of radius 1,
    with non random theta/phi
  */
  inline Vec3 random_vector_on_hemisphere(const Vec3& normal, FloatType arg1, FloatType arg2) {
    auto vec = random_vector_on_unit_sphere(arg1, arg2);

    if (glm::dot(vec, normal) < 0)
      return -vec;

    return vec;
  }

  /*
    Returns a normalized vector (probably) distributed uniformly on a cone
    Cones have a normal vector representing their forward direction and thete
    is its half-angle
  */
  inline Vec3 random_vector_on_cone(const Vec3& normal, FloatType theta) {
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

    auto rotmatrix = glm::mat4_cast(glm::angleAxis(angle, rotaxis));
    auto point4    = rotmatrix * Vec4(x, y, z, 1);

    if (normal.z > 0.999999)
      return Vec3(point4);

    if (normal.z < -0.999999)
      return -Vec3(point4);

    auto rotaxis2   = glm::normalize(glm::cross(normal, Vec3(0, 0, -1)));
    auto rotmatrix2 = glm::mat4_cast(glm::angleAxis(glm::acos(normal.z), rotaxis2));

    return Vec3(rotmatrix2 * point4);
  }

  /*
    Returns a normalized vector (probably) distributed uniformly on a cone
    Cones have a normal vector representing their forward direction and thete
    is its half-angle, with non-random args
  */
  inline Vec3 random_vector_on_cone(const Vec3& normal, FloatType theta, FloatType arg1,
                                    FloatType arg2) {

    auto z    = glm::cos(2 * theta);
    auto phi  = arg1 * 2 * M_PI;
    auto sqrt = glm::sqrt(1.0 - z * z);
    auto x    = sqrt * glm::cos(phi);
    auto y    = sqrt * glm::sin(phi);

    Vec3 rotaxis{glm::sin(phi), -glm::cos(phi), 0};
    auto angle = 2 * theta * (1 - 2 * glm::acos(arg2) / M_PI);

    auto rotmatrix = glm::mat4_cast(glm::angleAxis(angle, rotaxis));
    auto point4    = rotmatrix * Vec4(x, y, z, 1);

    if (normal.z > 0.999999)
      return Vec3(point4);

    if (normal.z < -0.999999)
      return -Vec3(point4);

    auto rotaxis2   = glm::normalize(glm::cross(normal, Vec3(0, 0, -1)));
    auto rotmatrix2 = glm::mat4_cast(glm::angleAxis(glm::acos(normal.z), rotaxis2));

    return Vec3(rotmatrix2 * point4);
  }

  /*
    Estimates the number of seconds to render Y samples when we've done X samples
  */
  inline double estimate_seconds_left(int num_samples_done, double seconds_passed,
                                      int sample_target) {
    auto samples_per_second = num_samples_done / seconds_passed;
    return (sample_target - num_samples_done) / samples_per_second;
  }

  /*
    Gets the next sample target
  */
  inline int next_sample_target(int current_samples) {
    if (current_samples == 0)
      return 10;

    auto step = glm::max(10.0, (double)current_samples * 0.1);
    return current_samples + step;
  }

  /*
    Formats seconds into hours, minutes and seconds
  */
  inline std::string format_time(int time_seconds) {
    std::ostringstream ss;

    auto hours   = time_seconds / 3600;
    auto minutes = (time_seconds / 60) % 60;
    auto seconds = time_seconds % 60;

    ss << std::setw(2) << std::setfill('0');
    ss << hours << ":";

    ss << std::setw(2) << std::setfill('0');
    ss << minutes << ":";

    ss << std::setw(2) << std::setfill('0');
    ss << seconds;

    return ss.str();
  }

  inline bool is_digit_1_to_9(char ch) { return ch >= '1' && ch <= '9'; }
  inline bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }

  inline bool is_hex_digit(char ch) {
    return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
  }

  inline bool is_control_char(char ch) { return ch < 32; }

  inline bool is_valid_escape(char ch) {
    switch (ch) {
    case '\\':
    case '"':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
    case 'u': return true;
    default: return false;
    }
  }

  inline bool is_valid_identifier_char(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') ||
           ch == '_' || ch == '$';
  }

  inline bool is_valid_first_identifier_char(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '$';
  }

  inline std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    size_t                   pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string              token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
      token     = s.substr(pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
  }

  inline std::optional<std::string> load_file_into_string(const std::string& filename) {
    std::ifstream f(filename, std::ios::binary);

    if (!f.good())
      return {};

    std::string ret;

    char buf[1024];
    while (!f.eof()) {
      auto read = f.readsome(buf, 1023);
      buf[read] = 0; // make sure it has a null termination
      ret += buf;

      if (!read)
        break;
    }

    return ret;
  }

} // namespace Oxy
