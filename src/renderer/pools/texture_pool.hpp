#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "renderer/material/texture.hpp"

namespace Oxy::Pools {

  using TextureHandle = const std::string&;

  using MutableTextureRef = Texture&;
  using TextureRef        = const Texture&;

  class TexturePool {
  public:
    MutableTextureRef make(TextureHandle handle) {
      auto texture = Texture();
      m_textures.emplace(handle, texture);
      return m_textures.at(handle);
    }

    std::optional<TextureRef> get(TextureHandle handle) {
      if (m_textures.contains(handle))
        return m_textures.at(handle);

      return {};
    }

  private:
    std::unordered_map<TextureHandle, Texture> m_textures;
  };

} // namespace Oxy::Pools
