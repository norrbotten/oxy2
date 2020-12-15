#pragma once

#include "renderer/material/color.hpp"
#include "renderer/sdl/utils.hpp"

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

  class ValidationError final : public std::exception {
  public:
    ValidationError(const std::string& err)
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
    Plane,
    Triangle,
  };

  struct ObjectDeclarationData {
    ObjectType type;

    std::string material;

    struct SphereParams {
      double center[3];
      double radius;
    };

    struct PlaneParams {
      double point[3];
      double normal[3];
    };

    struct TriangleParams {
      double p0[3];
      double p1[3];
      double p2[3];
    };

    std::variant<SphereParams, PlaneParams, TriangleParams> params;
  };

  struct CameraDeclarationData {
    double position[3];
    double aim[3];
    double fov;
  };

  struct RenderDeclarationData {
    int width;
    int height;
    int samples;
  };

  using KeyValue = std::unordered_map<std::string, std::string>;

  /*
    SDL execution context struct
  */
  struct ExecutionContext {
    ExecutionContext() {
      texture_defs.emplace("@default", TextureDeclarationData{"@default", ""});
      material_defs.emplace("@default",
                            MaterialDeclarationData{"@default", MaterialType::BSDF, "@default",
                                                    MaterialDeclarationData::BSDFParams{
                                                        {1.0, 1.0, 1.0}, 1.0, 0.0, 0.0, 0.0, 0.0}});
    }

    void validate() const {
      // check so all used textures exist
      for (auto& [name, material] : material_defs) {
        if (!texture_defs.contains(material.texture))
          throw ValidationError(fmt([&, name = name, texture = material.texture](auto& ss) {
            ss << "Material '" << name << "' referencing texture '" << texture
               << "' that doesn't exist";
          }));
      }

      // check so all used materials exist
      for (auto& object : object_defs) {
        if (!material_defs.contains(object.material))
          throw ValidationError(fmt([&, material = object.material](auto& ss) {
            ss << "Object referencing material '" << material << "' that doesn't exist";
          }));
      }
    }

    std::unordered_map<std::string, TextureDeclarationData>  texture_defs;
    std::unordered_map<std::string, MaterialDeclarationData> material_defs;
    std::vector<ObjectDeclarationData>                       object_defs;

    CameraDeclarationData camera;
    RenderDeclarationData render;
  };

} // namespace Oxy::SDL
