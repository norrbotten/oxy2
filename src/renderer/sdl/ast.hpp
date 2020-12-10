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

    virtual bool is_type_reference() const { return false; }
    virtual bool is_constant_reference() const { return true; }

    virtual bool is_texture_reference() const { return false; }
    virtual bool is_material_reference() const { return false; }
    virtual bool is_object_reference() const { return false; }
    virtual bool is_params_reference() const { return false; }

    virtual std::string stringify(int indent = 0) const { return "**unimplemented**"; }

  protected:
    std::vector<AST*> m_children;
  };

  class DeclarationNode : public AST {
  public:
    REF(decl);

  protected:
    KeyValue m_decl;
  };

  class SceneDeclarationNode final : public DeclarationNode {
  public:
    virtual void exec(ExecutionContext& ctx) override;

    virtual bool is_scene_declaration() const override { return true; }

    virtual std::string stringify(int indent = 0) const override {
      std::stringstream ss;

      ss << get_indent(indent) << "SceneDeclarationNode:\n";
      for (auto& child : m_children) {
        ss << get_indent(indent) << child->stringify(indent + 1);
      }
      ss << "\n";

      return ss.str();
    }
  };

  class TextureDeclarationNode final : public DeclarationNode {
  public:
    virtual void exec(ExecutionContext& ctx) override;
    virtual bool is_texture_declaration() const override { return true; }

    virtual std::string stringify(int indent = 0) const override {
      std::stringstream ss;

      ss << get_indent(indent) << "TextureDeclarationNode:\n";

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
  };

  class MaterialDeclarationNode final : public DeclarationNode {
  public:
    virtual void exec(ExecutionContext& ctx) override;
    virtual bool is_material_declaration() const override { return true; }

    virtual std::string stringify(int indent = 0) const override {
      std::stringstream ss;

      ss << get_indent(indent) << "MaterialDeclarationNode:\n";

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
  };

  class ObjectDeclarationNode final : public DeclarationNode {
  public:
    virtual void exec(ExecutionContext& ctx) override;
    virtual bool is_object_declaration() const override { return true; }

    virtual std::string stringify(int indent = 0) const override {
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
  };

} // namespace Oxy::SDL
