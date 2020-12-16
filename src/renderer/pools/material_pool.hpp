#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "renderer/material/bsdf.hpp"

namespace Oxy::Pools {

  using MaterialHandle = std::string;

  using MutableMaterialRef = BSDF*;

  using MaterialRef = BSDF*;

  class MaterialPool {
  public:
    ~MaterialPool() {
      for (auto& material : m_materials)
        delete material.second;
    }

    template <typename... Args>
    MutableMaterialRef make(MaterialHandle handle, Args... args) {
      auto material = new BSDF(args...);
      m_materials.emplace(handle, material);

      return m_materials.at(handle);
    }

    MutableMaterialRef make(MaterialHandle handle, BSDF* material) {
      m_materials.emplace(handle, material);
      return m_materials.at(handle);
    }

    std::optional<MaterialRef> get(MaterialHandle handle) const {
      if (m_materials.contains(handle))
        return m_materials.at(handle);

      return {};
    }

  private:
    std::unordered_map<MaterialHandle, BSDF*> m_materials;
  };

} // namespace Oxy::Pools
