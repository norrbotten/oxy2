#pragma once

#include <memory>

#include "renderer/material/impl/bsdf.hpp"
#include "renderer/material/impl/emissive.hpp"

namespace Oxy {

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

  auto create_emissive(Color energy) { return std::shared_ptr<Material>(new Emissive(energy)); }

  auto create_lambertian(Color albedo) { return create_bsdf(albedo, 1.0, 0.0, 0.0, 1.45, 0.0); }

  auto create_glossy(Color albedo, FloatType roughness) {
    return create_bsdf(albedo, roughness, 0.0, 0.0, 1.45, 0.0);
  }

  auto create_glass(Color albedo, FloatType roughness) {
    return create_bsdf(albedo, roughness, 0.0, 0.0, 1.45, 1.0);
  }

} // namespace Oxy
