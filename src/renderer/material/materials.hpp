#pragma once

#include "renderer/material/impl/bsdf.hpp"

namespace Oxy {

  BSDF* create_bsdf(Color albedo, FloatType roughness, FloatType clearcoat,
                    FloatType clearcoat_roughness, FloatType ior, FloatType transmission) {

    auto* mat = new BSDF(albedo);

    mat->roughness()           = roughness;
    mat->clearcoat()           = clearcoat;
    mat->clearcoat_roughness() = clearcoat_roughness;
    mat->ior()                 = ior;
    mat->transmission()        = transmission;

    return mat;
  }

  BSDF* create_diffuse(Color albedo) { return create_bsdf(albedo, 1.0, 0.0, 0.0, 1.45, 0.0); }

  BSDF* create_glossy(Color albedo, FloatType roughness) {
    return create_bsdf(albedo, roughness, 0.0, 0.0, 1.45, 0.0);
  }

  BSDF* create_glass(Color albedo, FloatType roughness) {
    return create_bsdf(albedo, roughness, 0.0, 0.0, 1.45, 1.0);
  }

} // namespace Oxy
