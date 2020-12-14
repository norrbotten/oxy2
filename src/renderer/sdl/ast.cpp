#include "renderer/sdl/ast.hpp"

namespace Oxy::SDL {

  void SceneDeclarationNode::exec(ExecutionContext& ctx) {
    for (auto& child : m_children)
      child->exec(ctx);
  }

  std::string SceneDeclarationNode::stringify(int indent) const {
    std::stringstream ss;

    ss << get_indent(indent) << "SceneDeclarationNode:\n";
    for (auto& child : m_children) {
      ss << get_indent(indent) << child->stringify(indent + 1);
    }
    ss << "\n";

    return ss.str();
  }

  //

  void TextureDeclarationNode::exec(ExecutionContext& ctx) {
    auto data = this->parse_into_data();

    if (data.name == "")
      throw ExecutionError(fmt([&](auto& ss) { ss << "Texture has unset name"; }));

    if (ctx.texture_defs.contains(data.name))
      throw ExecutionError(
          fmt([&](auto& ss) { ss << "Texture '" << data.name << "' is already declared"; }));

    ctx.texture_defs.emplace(data.name, data);
  }

  std::string TextureDeclarationNode::stringify(int indent) const {
    std::stringstream ss;

    ss << get_indent(indent) << "TextureDeclarationNode:\n";

    for (auto& [key, value] : m_decl) {
      ss << get_indent(indent + 1) << key << ": ";

      if (key == "params") {
        ss << "\n";

        for (auto it = m_nested_params.begin(); it != m_nested_params.end();) {
          ss << get_indent(indent + 2) << it->first << ": " << it->second;
          if (++it != m_nested_params.end())
            ss << "\n";
        }
      }
      else {
        if (auto pos = contains_newline(value); pos > 0)
          ss << value.substr(0, pos - 1) << "...";
        else
          ss << value;
      }

      ss << "\n";
    }

    return ss.str();
  }

  TextureDeclarationData TextureDeclarationNode::parse_into_data() const {
    TextureDeclarationData data;

    data.name = m_decl.contains("name") ? m_decl.at("name") : "";
    data.path = m_decl.contains("path") ? m_decl.at("path") : "";

    return data;
  }

  //

  void MaterialDeclarationNode::exec(ExecutionContext& ctx) {
    auto data = this->parse_into_data();

    if (data.name == "")
      throw ExecutionError(fmt([&](auto& ss) { ss << "Material has unset name"; }));

    if (data.type == MaterialType::Unset)
      throw ExecutionError(
          fmt([&](auto& ss) { ss << "Material '" << data.name << "' has unset or unknown type"; }));

    if (ctx.material_defs.contains(data.name))
      throw ExecutionError(
          fmt([&](auto& ss) { ss << "Material '" << data.name << "' is already declared"; }));

    ctx.material_defs.emplace(data.name, data);
  }

  std::string MaterialDeclarationNode::stringify(int indent) const {
    std::stringstream ss;

    ss << get_indent(indent) << "MaterialDeclarationNode:\n";

    for (auto& [key, value] : m_decl) {
      ss << get_indent(indent + 1) << key << ": ";

      if (key == "params") {
        ss << "\n";

        for (auto it = m_nested_params.begin(); it != m_nested_params.end();) {
          ss << get_indent(indent + 2) << it->first << ": " << it->second;
          if (++it != m_nested_params.end())
            ss << "\n";
        }
      }
      else {
        if (auto pos = contains_newline(value); pos > 0)
          ss << value.substr(0, pos - 1) << "...";
        else
          ss << value;
      }

      ss << "\n";
    }

    return ss.str();
  }

  MaterialDeclarationData MaterialDeclarationNode::parse_into_data() const {
    MaterialDeclarationData data;

    data.name = m_decl.contains("name") ? m_decl.at("name") : "";
    auto type = m_decl.contains("type") ? m_decl.at("type") : "";

    data.texture = m_decl.contains("texture") ? m_decl.at("texture") : "@default";

    if (type == "")
      data.type = MaterialType::Unset;
    else {
      if (type == "bsdf") {
        data.type = MaterialType::BSDF;
        auto bsdf = MaterialDeclarationData::BSDFParams();

        if (!(m_nested_params.contains("albedo") &&
              parse_float_triplet(bsdf.albedo, m_nested_params.at("albedo")))) {
          bsdf.albedo[0] = 1.0;
          bsdf.albedo[1] = 1.0;
          bsdf.albedo[2] = 1.0;
        }

        if (!(m_nested_params.contains("roughness") &&
              parse_float(&bsdf.roughness, m_nested_params.at("roughness"))))
          bsdf.roughness = 1.0;

        if (!(m_nested_params.contains("clearcoat") &&
              parse_float(&bsdf.clearcoat, m_nested_params.at("clearcoat"))))
          bsdf.clearcoat = 0.0;

        if (!(m_nested_params.contains("clearcoat_roughness") &&
              parse_float(&bsdf.clearcoat_roughness, m_nested_params.at("clearcoat_roughness"))))
          bsdf.clearcoat_roughness = 0.0;

        if (!(m_nested_params.contains("ior") && parse_float(&bsdf.ior, m_nested_params.at("ior"))))
          bsdf.ior = 0.0;

        if (!(m_nested_params.contains("transmission") &&
              parse_float(&bsdf.transmission, m_nested_params.at("transmission"))))
          bsdf.transmission = 0.0;

        data.params = bsdf;
      }
      else if (type == "emissive") {
        data.type     = MaterialType::Emission;
        auto emissive = MaterialDeclarationData::EmissiveParams();

        if (!(m_nested_params.contains("energy") &&
              parse_float_triplet(emissive.energy, m_nested_params.at("energy")))) {
          emissive.energy[0] = 1.0;
          emissive.energy[1] = 1.0;
          emissive.energy[2] = 1.0;
        }

        data.params = emissive;
      }
    }

    return data;
  }

  //

  void ObjectDeclarationNode::exec(ExecutionContext& ctx) {
    auto data = this->parse_into_data();

    if (data.type == ObjectType::Unset)
      throw ExecutionError(fmt([&](auto& ss) { ss << "Object with unset or unknown type"; }));

    ctx.object_defs.push_back(data);
  }

  std::string ObjectDeclarationNode::stringify(int indent) const {
    std::stringstream ss;

    ss << get_indent(indent) << "ObjectDeclarationNode:\n";

    for (auto& [key, value] : m_decl) {
      ss << get_indent(indent + 1) << key << ": ";

      if (key == "params") {
        ss << "\n";

        for (auto it = m_nested_params.begin(); it != m_nested_params.end();) {
          ss << get_indent(indent + 2) << it->first << ": " << it->second;
          if (++it != m_nested_params.end())
            ss << "\n";
        }
      }
      else {
        if (auto pos = contains_newline(value); pos > 0)
          ss << value.substr(0, pos - 1) << "...";
        else
          ss << value;
      }

      ss << "\n";
    }

    auto decl_data = parse_into_data();
    if (decl_data.type == ObjectType::Sphere) {
      auto params = std::get<ObjectDeclarationData::SphereParams>(decl_data.params);
      std::cout << params.radius << "\n";
    }

    return ss.str();
  }

  ObjectDeclarationData ObjectDeclarationNode::parse_into_data() const {
    ObjectDeclarationData data;

    auto type = m_decl.contains("type") ? m_decl.at("type") : "";

    data.material = m_decl.contains("material") ? m_decl.at("material") : "@default";

    if (type == "sphere") {
      data.type   = ObjectType::Sphere;
      auto sphere = ObjectDeclarationData::SphereParams();

      if (!(m_nested_params.contains("center") &&
            parse_float_triplet(sphere.center, m_nested_params.at("center")))) {
        sphere.center[0] = 0.0;
        sphere.center[1] = 0.0;
        sphere.center[2] = 0.0;
      }

      if (!(m_nested_params.contains("radius") &&
            parse_float(&sphere.radius, m_nested_params.at("radius"))))
        sphere.radius = 1.0;

      data.params = sphere;
    }
    else if (type == "plane") {
      data.type  = ObjectType::Plane;
      auto plane = ObjectDeclarationData::PlaneParams();

      if (!(m_nested_params.contains("point") &&
            parse_float_triplet(plane.point, m_nested_params.at("point")))) {
        plane.point[0] = 0.0;
        plane.point[1] = 0.0;
        plane.point[2] = 0.0;
      }

      if (!(m_nested_params.contains("normal") &&
            parse_float_triplet(plane.normal, m_nested_params.at("normal")))) {
        plane.normal[0] = 0.0;
        plane.normal[1] = 0.0;
        plane.normal[2] = 1.0;
      }

      data.params = plane;
    }
    else {
      data.type = ObjectType::Unset;
    }

    return data;
  }

  void CameraDeclarationNode::exec(ExecutionContext& ctx) { ctx.camera = this->parse_into_data(); }

  std::string CameraDeclarationNode::stringify(int indent) const {
    std::stringstream ss;

    ss << get_indent(indent) << "ObjectDeclarationNode:\n";

    for (auto& [key, value] : m_decl) {
      ss << get_indent(indent + 1) << key << ": ";

      if (auto pos = contains_newline(value); pos > 0)
        ss << value.substr(0, pos - 1) << "...";
      else
        ss << value;

      ss << "\n";
    }

    return ss.str();
  }

  CameraDeclarationData CameraDeclarationNode::parse_into_data() const {
    CameraDeclarationData data;

    if (!(m_decl.contains("position") &&
          parse_float_triplet(data.position, m_decl.at("position")))) {
      data.position[0] = 0.0;
      data.position[1] = 0.0;
      data.position[2] = 0.0;
    }

    if (!(m_decl.contains("aim") && parse_float_triplet(data.aim, m_decl.at("aim")))) {
      data.aim[0] = 0.0;
      data.aim[1] = 0.0;
      data.aim[2] = 0.0;
    }

    if (!(m_decl.contains("fov") && parse_float(&data.fov, m_decl.at("fov"))))
      data.fov = 60.0;

    return data;
  }

} // namespace Oxy::SDL
