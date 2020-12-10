#pragma once

#include "renderer/material/color.hpp"

#include <exception>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Oxy::SDL {

  class ExecutionError final : public std::exception {
  public:
    ExecutionError(const std::string& err)
        : m_error(err) {}

    virtual const char* what() const noexcept { return m_error.c_str(); }

  private:
    std::string m_error;
  };

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

    std::string texture;

    struct EmissiveParams {
      double energy[3];
    };

    struct BSDFParams {
      double albedo[3];
      double roughness;
      double clearcoat;
      double clearcoat_roughness;
      double ior;
      double transmission;
    };

    std::variant<EmissiveParams, BSDFParams> params;
  };

  enum class ObjectType {
    Unset,
    Sphere,
  };

  struct ObjectDeclarationData {
    ObjectType type;

    std::string material;

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
    std::vector<ObjectDeclarationData>                       object_defs;
  };

} // namespace Oxy::SDL
