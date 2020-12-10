#include "renderer/sdl/ast.hpp"

namespace Oxy::SDL {

  void SceneDeclarationNode::exec(ExecutionContext& ctx) {
    for (auto& child : m_children)
      child->exec(ctx);
  }

  void TextureDeclarationNode::exec(ExecutionContext& ctx) {}

  void MaterialDeclarationNode::exec(ExecutionContext& ctx) {}

  void ObjectDeclarationNode::exec(ExecutionContext& ctx) {}

} // namespace Oxy::SDL
