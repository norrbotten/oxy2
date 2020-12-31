#pragma once

#include "renderer/tracing/environment.hpp"

namespace Oxy::Tracing {

  struct NishitaParameters {
    Vec3      sun_direction{1, 1, -0.7};
    FloatType earth_radius{6300e3};
    FloatType atmosphere_radius{6400e3};
    FloatType rayleigh_scale_height{7994};
    FloatType mie_scale_height{1200};
    Vec3      beta_r{3.8e-6, 13.5e-6, 33.1e-6};
    Vec3      beta_m{21e-6, 21e-6, 21e-6};
    FloatType multiplier{1};
  };

  class NishitaEnvironment : public EnvironmentMap {
  public:
    NishitaEnvironment(const NishitaParameters& parameters = {})
        : m_params(parameters) {

      m_params.sun_direction = glm::normalize(m_params.sun_direction);
    }

    virtual Color sample(const SingleRay& ray) const override {
      FloatType t_near, t_far;

      if (!isect_sky(ray, t_near, t_far)) {
        return Color();
      }

      // we are almost always inside the atmosphere, so t_near is gonna be negative
      if (t_near < 0)
        t_near = 0;

      constexpr auto atmosphere_samples = 12;
      constexpr auto sun_samples        = 6;

      auto segment_length = (t_far - t_near) / atmosphere_samples;

      Vec3 sum_r{}, sum_m{};

      FloatType optical_depth_r = 0, optical_depth_m = 0;

      for (int i = 0; i < atmosphere_samples; i++) {
        auto er = m_params.earth_radius;

        auto sample_pos    = ray.origin + ray.direction * (FloatType)i * segment_length * 0.5;
        auto sample_height = er + sample_pos.z;

        auto hr = glm::exp(-(sample_height - er) / m_params.rayleigh_scale_height) * segment_length;
        auto hm = glm::exp(-(sample_height - er) / m_params.mie_scale_height) * segment_length;

        optical_depth_r += hr;
        optical_depth_m += hm;

        FloatType dummy, t_sun = 0;
        isect_sky(SingleRay{sample_pos, -m_params.sun_direction}, dummy, t_sun);
        auto segment_length_sun = t_sun / sun_samples;

        FloatType optical_depth_sun_r = 0, optical_depth_sun_m = 0;

        for (int j = 0; j < sun_samples; j++) {
          auto sun_sample_height =
              sample_height - m_params.sun_direction.z * segment_length_sun * (FloatType)j * 0.5;

          optical_depth_sun_r +=
              glm::exp(-(sun_sample_height - er) / m_params.rayleigh_scale_height);

          optical_depth_sun_m += glm::exp(-(sun_sample_height - er) / m_params.mie_scale_height);
        }

        auto tau = m_params.beta_r * (optical_depth_sun_r + optical_depth_sun_r) +
                   m_params.beta_m * 1.1 * (optical_depth_m + optical_depth_sun_m);

        auto attenuation = Vec3{glm::exp(-tau.x), glm::exp(-tau.y), glm::exp(-tau.z)};

        sum_r += attenuation * hr;
        sum_m += attenuation * hm;
      }

      auto mu      = glm::dot(ray.direction, -m_params.sun_direction);
      auto phase_r = 3.0 / (16.0 / M_PI) * (1.0 + mu * mu);

      auto g       = 0.76;
      auto phase_m = 3.0 / (8.0 * M_PI) * ((1.0 - g * g) * (1.0 + mu * mu)) /
                     ((2.0 + g * g) * glm::pow(1.0 + g * g - 2.0 * g * mu, 1.5));

      auto ret = m_params.multiplier *
                 (sum_r * m_params.beta_r * phase_r + sum_m * m_params.beta_m * phase_m);

      return Color(ret);
    }

  private:
    inline bool isect_sky(const SingleRay& ray, FloatType& t_near, FloatType& t_far) const {
      FloatType a = glm::dot(ray.direction, ray.direction);
      FloatType b = 2.0 * glm::dot(ray.origin, ray.direction);
      FloatType c = glm::dot(ray.origin, ray.origin) -
                    m_params.atmosphere_radius * m_params.atmosphere_radius;

      FloatType disc = glm::sqrt(b * b - 4 * a * c);

      if (disc > 0) {
        auto t0 = (-b - disc) / (2 * a);
        auto t1 = (-b + disc) / (2 * a);

        if (t0 > t1) {
          t_near = t1;
          t_far  = t0;
          return true;
        }

        t_near = t0;
        t_far  = t1;
        return true;
      }

      return false;
    }

  private:
    NishitaParameters m_params;
  };

} // namespace Oxy::Tracing
