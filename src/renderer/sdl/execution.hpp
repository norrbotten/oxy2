#pragma once

#include "renderer/material/color.hpp"

#include <string>
#include <unordered_map>
#include <variant>

namespace Oxy::SDL {

  /*
    Declaration data structs, this is data from the source file which means
    some fields could be empty/not set
  */

  struct TextureDeclarationData {
    std::string name;
    std::string path;
  };

  enum class MaterialType {
    Unset,
    BSDF,
    Emission,
  };

  struct MaterialDeclarationData {
    std::string  name;
    MaterialType type;

    struct EmissiveParams {
      double energy[3];
    };

    struct BSDFParams {
      double bsdf_albedo[3];
      double bsdf_roughness;
      double bsdf_clearcoat;
      double bsdf_clearcoat_roughness;
      double bsdf_ior;
      double bsdf_transmission;
    };

    std::variant<EmissiveParams, BSDFParams> params;
  };

  enum class ObjectType {
    Unset,
    Sphere,
  };

  struct ObjectDeclarationData {
    ObjectType type;

    MaterialDeclarationData material_def;

    struct SphereParams {
      double center[3];
      double radius;
    };

    std::variant<SphereParams> params;
  };

  using KeyValue = std::unordered_map<std::string, std::string>;

  /*
    SDL execution context struct
  */
  struct ExecutionContext {
    std::unordered_map<std::string, TextureDeclarationData>  texture_defs;
    std::unordered_map<std::string, MaterialDeclarationData> materials_defs;
    std::unordered_map<std::string, ObjectDeclarationData>   object_defs;

    void push_error(const std::string& msg) {
      // TODO
    }

    // Declares a texture, returns true on failure
    bool declare_texture(const KeyValue& base_params) {
      if (!base_params.contains("name")) {
        push_error("No name on texture declaration");
        return true;
      }

      auto& name = base_params.at("name");

      if (texture_defs.contains("name")) {
        push_error("Texture with the same name already exists");
        return true;
      }

      TextureDeclarationData texture;
      texture.name = name;
      texture.path = "bigbong";

      texture_defs.emplace(texture.name, texture);

      return false;
    }

    // Declares a material from base params and detail, returns true on failure
    bool declare_material(const KeyValue& base_params, const KeyValue& detail_params) {
      return false;
    }

    // Declares an object from base params and detail, returns true on failure
    bool declare_object(const KeyValue& base_params, const KeyValue& detail_params) {
      return false;
    }
  };

} // namespace Oxy::SDL
