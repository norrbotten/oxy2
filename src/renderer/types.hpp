#pragma once

#include <optional>

#include <glm/glm.hpp>

namespace Oxy {

  using FloatType = double;

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

} // namespace Oxy
