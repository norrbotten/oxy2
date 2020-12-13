#pragma once

#include "renderer/sdl/parser.hpp"

#include "renderer/tracing/world.hpp"

#include "renderer/pools/material_pool.hpp"
#include "renderer/pools/mesh_pool.hpp"
#include "renderer/pools/texture_pool.hpp"

namespace Oxy {

  namespace fs = std::filesystem;

  class Renderer {
  public:
    std::optional<std::string> load_file(fs::path filename);

  private:
    void build_scene_from_valid_ctx(const SDL::ExecutionContext& ctx);

  private:
    Tracing::World m_world;

    Pools::MaterialPool m_material_pool;
    Pools::TexturePool  m_texture_pool;
  };

} // namespace Oxy
