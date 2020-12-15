#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "renderer/material/material.hpp"

namespace Oxy::Pools {

  using MaterialHandle = std::string;

  using MutableMaterialRef = Material*;

  using MaterialRef = Material*;

  class MaterialPool {
  public:
    ~MaterialPool() {
      for (auto& material : m_materials)
        delete material.second;
    }

    template <typename Derived, typename... Args>
    MutableMaterialRef make(MaterialHandle handle, Args... args) {
      auto material = new Derived(args...);
      m_materials.emplace(handle, material);

      return ((Material*)m_materials.at(handle));
    }

    MutableMaterialRef make(MaterialHandle handle, Material* material) {
      m_materials.emplace(handle, material);
      return m_materials.at(handle);
    }

    std::optional<MaterialRef> get(MaterialHandle handle) const {
      if (m_materials.contains(handle))
        return ((Material*)m_materials.at(handle));

      return {};
    }

  private:
    std::unordered_map<MaterialHandle, Material*> m_materials;
  };

} // namespace Oxy::Pools
