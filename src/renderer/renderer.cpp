#include "renderer/renderer.hpp"

#include <fstream>
#include <streambuf>

namespace Oxy {

  std::optional<std::string> Renderer::load_file(fs::path filename) {
    std::ifstream f(filename);

    if (!f.good()) {
      return "File not found/invalid";
    }

    std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    auto parser = SDL::Parser(str);

    SDL::SceneDeclarationNode* scene_decl;

    try {
      scene_decl = parser.parse();
    }
    catch (const SDL::ParsingError& err) {
      return std::string("Parsing error: ") + err.what();
    }

    auto ctx = SDL::ExecutionContext();

    try {
      scene_decl->exec(ctx);
    }
    catch (const SDL::ExecutionError& err) {
      return std::string("Exec error: ") + err.what();
    }

    try {
      ctx.validate();
    }
    catch (const SDL::ValidationError& err) {
      return std::string("Validation error: ") + err.what();
    }

    build_scene_from_valid_ctx(ctx);

    return {};
  }

  void Renderer::build_scene_from_valid_ctx(const SDL::ExecutionContext& ctx) {
    for (auto& [name, texture] : ctx.texture_defs) {
      auto tex = m_texture_pool.make(name);

      if (!(texture.path != "" && tex->load_from_file(texture.path)))
        tex->make_solid_color(Color(1.0, 1.0, 1.0));
    }

    for (auto& [name, material] : ctx.material_defs) {
      switch (material.type) {
      case SDL::MaterialType::BSDF: {
        auto& params = std::get<SDL::MaterialDeclarationData::BSDFParams>(material.params);
        auto  mater  = m_material_pool.make<BSDF>(name);

        mater->set_texture(m_texture_pool, material.texture);

        mater->albedo()              = Color(params.albedo[0], params.albedo[1], params.albedo[2]);
        mater->roughness()           = params.roughness;
        mater->clearcoat()           = params.clearcoat;
        mater->clearcoat_roughness() = params.clearcoat_roughness;
        mater->ior()                 = params.ior;
        mater->transmission()        = params.transmission;

      } break;

      case SDL::MaterialType::Emission: {
        auto& params = std::get<SDL::MaterialDeclarationData::EmissiveParams>(material.params);
        auto  mater  = m_material_pool.make<Emissive>(name);

        mater->energy() = Color(params.energy[0], params.energy[1], params.energy[2]);

      } break;

      case SDL::MaterialType::Unset: assert(false); break;
      }
    }

    for (auto& object : ctx.object_defs) {
      switch (object.type) {
      case SDL::ObjectType::Sphere: {
        auto& params = std::get<SDL::ObjectDeclarationData::SphereParams>(object.params);
        auto  obj    = new Tracing::TracableSphere(
            Vec3(params.center[0], params.center[1], params.center[2]), params.radius);

        obj->material() = m_material_pool.get<Material>(object.material).value();

        m_integrator->world().add_object(obj);
      } break;

      case SDL::ObjectType::Unset: assert(false); break;
      }
    }
  }

} // namespace Oxy