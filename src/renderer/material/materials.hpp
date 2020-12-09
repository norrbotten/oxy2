#pragma once

#include <memory>

#include "renderer/material/impl/bsdf.hpp"
#include "renderer/material/impl/emissive.hpp"

namespace Oxy {

  /*
    BSDF factory
  */
  auto create_bsdf(Color albedo, FloatType roughness, FloatType clearcoat,
                   FloatType clearcoat_roughness, FloatType ior, FloatType transmission) {

    auto* mat = new BSDF(albedo);

    mat->roughness()           = roughness;
    mat->clearcoat()           = clearcoat;
    mat->clearcoat_roughness() = clearcoat_roughness;
    mat->ior()                 = ior;
    mat->transmission()        = transmission;

    return std::shared_ptr<Material>(mat);
  }

  /*
    Emissive material helper
  */
  auto create_emissive(Color energy) { return std::shared_ptr<Material>(new Emissive(energy)); }

  /*
    Lambertian material helper
    Lambertian is a 100% diffuse material
  */
  auto create_lambertian(Color albedo) { return create_bsdf(albedo, 1.0, 0.0, 0.0, 1.45, 0.0); }

  /*
    Glossy material helper
    Glossy is like lambertian but it scatters in the cone (hitnormal, roughness * PI/4)
    roughness=0 will give a perfect mirror, roughness=1 gives a lambertian material
  */
  auto create_glossy(Color albedo, FloatType roughness) {
    return create_bsdf(albedo, roughness, 0.0, 0.0, 1.45, 0.0);
  }

  /*
    Glass material helper
    Returns a material resembling glass with a certain roughness
  */
  auto create_glass(Color albedo, FloatType roughness) {
    return create_bsdf(albedo, roughness, 0.0, 0.0, 1.45, 1.0);
  }

} // namespace Oxy
