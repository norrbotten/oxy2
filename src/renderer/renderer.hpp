#pragma once

#include "renderer/sdl/parser.hpp"

#include "renderer/integrators/naive.hpp"
#include "renderer/integrators/naive_implicit.hpp"
#include "renderer/integrators/preview.hpp"
#include "renderer/integrators/pssmlt.hpp"

#include "renderer/tracing/objects.hpp"

#include "renderer/pools/material_pool.hpp"
#include "renderer/pools/mesh_pool.hpp"
#include "renderer/pools/texture_pool.hpp"

#include "renderer/timer.hpp"

#include "renderer/camera.hpp"
#include "renderer/image.hpp"
#include "renderer/sample_film.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  class Renderer {
  public:
    Renderer()
        : m_integrator(new Integrators::NaiveIntegrator()) {}

    std::optional<std::string> load_file(fs::path filename);

    void setup_integrator() {
      Integrators::IntegratorSetupContext ctx;
      ctx.width  = m_width;
      ctx.height = m_height;
      m_integrator->setup(ctx);
    }

    double sample_once() {
      Timer timer;
      timer.start();

      m_integrator->pre_pass();

      m_camera.for_each_pixel(m_film, [&](int x, int y, const SingleRay& ray) {
        return m_integrator->radiance(ray, x, y);
      });

      m_integrator->post_pass();

      return timer.elapsed_seconds();
    }

    void save_png(const std::string& filename) {
      Image image(m_film);
      image.extended_reinhard().gamma(2.2).clamp(0.0, 1.0).write_png(filename.c_str());
    }

    REF(film);
    REF(camera);

    auto& world() { return m_integrator->world(); }

    CREF(max_samples);
    CREF(texture_pool);
    CREF(material_pool);

  private:
    void build_scene_from_valid_ctx(const SDL::ExecutionContext& ctx);

  private:
    Integrators::Integrator* m_integrator;

    SampleFilm m_film;
    Camera     m_camera;

    Pools::TexturePool  m_texture_pool;
    Pools::MaterialPool m_material_pool;

    int m_width       = 1024;
    int m_height      = 1024;
    int m_max_samples = 128;
  };

} // namespace Oxy
