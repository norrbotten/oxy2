#pragma once

#include "renderer/tracing/world.hpp"

namespace Oxy::Integrators {

  struct IntegratorSetupContext {
    int width;
    int height;
  };

  /*
    Abstract Integrator class, used to compute the radiance along a ray
    Integrator owns the world instance, where all object data is stored
  */
  class Integrator {
  public:
    virtual Color radiance(const SingleRay& ray, int x, int y) = 0;

    virtual void setup(const IntegratorSetupContext&) {} // Called upon renderer initialization
    virtual void pre_pass() {}                           // Called before each render pass
    virtual void post_pass() {}                          // Called after each render pass

    REF(world);

  protected:
    Tracing::World m_world;
  };

} // namespace Oxy::Integrators