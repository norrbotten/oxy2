#pragma once

#include <iostream>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include "renderer/macros.hpp"

namespace Oxy {

  using FloatType = double;

  using Vec2 = glm::vec<2, FloatType>;
  using Vec3 = glm::vec<3, FloatType>;
  using Vec4 = glm::vec<4, FloatType>;
  using Mat3 = glm::mat<3, 3, FloatType>;
  using Mat4 = glm::mat<4, 4, FloatType>;

  using IntersectDistance = FloatType;
  using IntersectResult   = std::optional<IntersectDistance>;

  struct SingleRay {
    Vec3 origin;
    Vec3 direction;
  };

  struct BoundingBox {
    Vec3 min;
    Vec3 max;
  };

  inline std::ostream& operator<<(std::ostream& ofs, const Vec2& vec) {
    ofs << "[";

    for (int n = 0; n < 2 - 1; n++)
      ofs << vec[n] << ", ";

    ofs << vec[2 - 1] << "]";

    return ofs;
  }

  inline std::ostream& operator<<(std::ostream& ofs, const Vec3& vec) {
    ofs << "[";

    for (int n = 0; n < 3 - 1; n++)
      ofs << vec[n] << ", ";

    ofs << vec[3 - 1] << "]";

    return ofs;
  }

  inline std::ostream& operator<<(std::ostream& ofs, const Vec4& vec) {
    ofs << "[";

    for (int n = 0; n < 4 - 1; n++)
      ofs << vec[n] << ", ";

    ofs << vec[4 - 1] << "]";

    return ofs;
  }

} // namespace Oxy
