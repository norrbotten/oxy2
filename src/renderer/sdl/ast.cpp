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

  void TextureDeclarationNode::exec(ExecutionContext& ctx) {}

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

  void MaterialDeclarationNode::exec(ExecutionContext& ctx) {}

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

    if (type == "")
      data.type = MaterialType::Unset;
    else {
      if (type == "bsdf") {
        data.type   = MaterialType::BSDF;
        data.params = MaterialDeclarationData::BSDFParams();
      }
      else if (type == "emissive") {
        data.type   = MaterialType::Emission;
        data.params = MaterialDeclarationData::EmissiveParams();
      }
    }

    return data;
  }

  //

  void ObjectDeclarationNode::exec(ExecutionContext& ctx) {}

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

    return ss.str();
  }

} // namespace Oxy::SDL
