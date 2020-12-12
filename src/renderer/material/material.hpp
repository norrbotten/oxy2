#pragma once

#include "renderer/material/color.hpp"
#include "renderer/utils.hpp"

namespace Oxy {

  // A ray of light
  struct LightRay {
    SingleRay ray;
    Color     energy;
  };

  /*
    Abstract material class, a material needs to be able to scatter an incoming
    ray and return
  */
  class Material {
  public:
    virtual ~Material() {}

    /*
      Samples the materials albedo
    */
    virtual Color sample(const IntersectionContext& ctx) const = 0;

    /*
      Scatters the incoming ray and returns a new ray and the absorbtion mask/emission
    */
    virtual LightRay scatter(const IntersectionContext& ctx) const = 0;

    virtual bool is_emissive() const { return false; }
  };

} // namespace Oxy
