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
        auto  mater  = (BSDF*)m_material_pool.make<BSDF>(name);

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
        auto  mater  = (Emissive*)m_material_pool.make<Emissive>(name);

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

        obj->material() = m_material_pool.get(object.material).value();

        m_integrator->world().add_object(obj);
      } break;

      case SDL::ObjectType::Plane: {
        auto& params = std::get<SDL::ObjectDeclarationData::PlaneParams>(object.params);
        auto  obj =
            new Tracing::TracablePlane(Vec3(params.point[0], params.point[1], params.point[2]),
                                       Vec3(params.normal[0], params.normal[1], params.normal[2]));

        obj->material() = m_material_pool.get(object.material).value();

        m_integrator->world().add_object(obj);
      } break;

      case SDL::ObjectType::Triangle: {
        auto& params = std::get<SDL::ObjectDeclarationData::TriangleParams>(object.params);
        auto  obj    = new Tracing::TracableTriangle(Vec3(params.p0[0], params.p0[1], params.p0[2]),
                                                 Vec3(params.p1[0], params.p1[1], params.p1[2]),
                                                 Vec3(params.p2[0], params.p2[1], params.p2[2]));

        obj->material() = m_material_pool.get(object.material).value();

        m_integrator->world().add_object(obj);
      } break;

      case SDL::ObjectType::Unset: assert(false); break;
      }
    }

    auto& cam = ctx.camera;
    m_camera.set_pos(Vec3(cam.position[0], cam.position[1], cam.position[2]));
    m_camera.aim(Vec3(cam.aim[0], cam.aim[1], cam.aim[2]));
    m_camera.set_fov(cam.fov);

    auto& render  = ctx.render;
    m_width       = render.width;
    m_height      = render.height;
    m_max_samples = render.samples;

    m_film.resize(render.width, render.height);
  }

} // namespace Oxy
