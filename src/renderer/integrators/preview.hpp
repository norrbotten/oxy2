#pragma once

#include "renderer/integrators/integrator.hpp"

namespace Oxy::Integrators {

  enum class PreviewMode {
    FlatAlbedo, // Flat colors
    Albedo,     // Shaded colors
    Flashlight, // B/W shaded
    Normal,     // Show normal vectors (XYZ -> RGB)
  };

  /*
    Preview integrator, for rendering a fast preview image
  */
  class PreviewIntegrator final : public Integrator {
  public:
    PreviewIntegrator(PreviewMode mode = PreviewMode::Albedo)
        : m_preview_mode(mode) {}

    virtual Color radiance(const SingleRay& ray, int, int) override {
      auto isect = m_world.intersect_ray(ray);

      if (isect.hit) {
        switch (m_preview_mode) {
        case PreviewMode::FlatAlbedo: return isect.object->material()->sample(isect);

        case PreviewMode::Albedo:
          return isect.object->material()->sample(isect) *
                 glm::abs(glm::dot(ray.direction, -isect.hitnormal));

        case PreviewMode::Flashlight: return Color(glm::dot(ray.direction, -isect.hitnormal));

        case PreviewMode::Normal:
          return Color(0.5 * isect.hitnormal.x + 0.5, 0.5 * isect.hitnormal.y + 0.5,
                       0.5 * isect.hitnormal.z + 0.5);
        }
      }

      return Color();
    }

  private:
    PreviewMode m_preview_mode;
  };

} // namespace Oxy::Integrators