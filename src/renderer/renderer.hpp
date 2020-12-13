#pragma once

#include "renderer/sdl/parser.hpp"

#include "renderer/integrators/naive.hpp"

#include "renderer/tracing/objects.hpp"

#include "renderer/pools/material_pool.hpp"
#include "renderer/pools/mesh_pool.hpp"
#include "renderer/pools/texture_pool.hpp"

#include "renderer/material/materials.hpp"

#include "renderer/timer.hpp"

#include "renderer/camera.hpp"
#include "renderer/image.hpp"
#include "renderer/sample_film.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  class Renderer {
  public:
    Renderer()
        : m_integrator(new Integrators::Naive()) {}

    std::optional<std::string> load_file(fs::path filename);

    double sample_once() {
      Timer timer;
      timer.start();

      m_camera.for_each_pixel(m_film,
                              [&](const SingleRay& ray) { return m_integrator->radiance(ray); });

      return timer.elapsed_seconds();
    }

    void save_png(const std::string& filename) {
      Image image(m_film);
      image.extended_reinhard().gamma(2.2).clamp(0.0, 1.0).write_png(filename.c_str());
    }

    REF(film);
    REF(camera);

  private:
    void build_scene_from_valid_ctx(const SDL::ExecutionContext& ctx);

  private:
    Integrators::Integrator* m_integrator;

    SampleFilm m_film;
    Camera     m_camera;

    Pools::TexturePool  m_texture_pool;
    Pools::MaterialPool m_material_pool;
  };

} // namespace Oxy
