#pragma once

#include <type_traits>
#include <vector>

#include "renderer/sdl/execution.hpp"
#include "renderer/sdl/utils.hpp"

namespace Oxy::SDL {

  class AST {
  public:
    virtual ~AST() {
      for (auto& child : m_children)
        delete child;
    }

    template <typename T, typename = std::enable_if<std::is_base_of<AST, T>::value>>
    void push(T* child) {
      m_children.push_back((AST*)child);
    }

    virtual void exec(ExecutionContext& ctx) = 0;

    virtual bool is_scene_declaration() const { return false; }

    virtual bool is_constant_declaration() const { return false; }
    virtual bool is_type_declaration() const { return false; }

    virtual bool is_texture_declaration() const { return false; }
    virtual bool is_material_declaration() const { return false; }
    virtual bool is_object_declaration() const { return false; }
    virtual bool is_params_declaration() const { return false; }

    virtual bool is_camera_declaration() const { return false; }
    virtual bool is_render_declaration() const { return false; }

    virtual bool is_type_reference() const { return false; }
    virtual bool is_constant_reference() const { return true; }

    virtual bool is_texture_reference() const { return false; }
    virtual bool is_material_reference() const { return false; }
    virtual bool is_object_reference() const { return false; }
    virtual bool is_params_reference() const { return false; }

    virtual std::string stringify(int indent = 0) const {
      (void)indent;
      return "**unimplemented**";
    }

  protected:
    std::vector<AST*> m_children;
  };

  class DeclarationNode : public AST {
  public:
    REF(decl);
    REF(nested_params);

  protected:
    KeyValue m_decl;
    KeyValue m_nested_params;
  };

  class SceneDeclarationNode final : public DeclarationNode {
  public:
    virtual bool is_scene_declaration() const override { return true; }

    virtual void exec(ExecutionContext& ctx) override;

    virtual std::string stringify(int indent = 0) const override;
  };

  class TextureDeclarationNode final : public DeclarationNode {
  public:
    virtual bool is_texture_declaration() const override { return true; }

    virtual void exec(ExecutionContext& ctx) override;

    virtual std::string stringify(int indent = 0) const override;

    TextureDeclarationData parse_into_data() const;
  };

  class MaterialDeclarationNode final : public DeclarationNode {
  public:
    virtual bool is_material_declaration() const override { return true; }

    virtual void        exec(ExecutionContext& ctx) override;
    virtual std::string stringify(int indent = 0) const override;

    MaterialDeclarationData parse_into_data() const;
  };

  class ObjectDeclarationNode final : public DeclarationNode {
  public:
    virtual bool is_object_declaration() const override { return true; }

    virtual void exec(ExecutionContext& ctx) override;

    virtual std::string stringify(int indent = 0) const override;

    ObjectDeclarationData parse_into_data() const;
  };

  class CameraDeclarationNode final : public AST {
  public:
    virtual bool is_camera_declaration() const override { return true; }

    virtual void exec(ExecutionContext& ctx) override;

    virtual std::string stringify(int indent = 0) const override;

    CameraDeclarationData parse_into_data() const;

    REF(decl);

  private:
    KeyValue m_decl;
  };

  class RenderDeclarationNode final : public AST {
  public:
    virtual bool is_render_declaration() const override { return true; }

    virtual void exec(ExecutionContext& ctx) override;

    virtual std::string stringify(int indent = 0) const override;

    RenderDeclarationData parse_into_data() const;

    REF(decl);

  private:
    KeyValue m_decl;
  };

} // namespace Oxy::SDL
