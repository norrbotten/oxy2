#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "renderer/material/material.hpp"

namespace Oxy::Pools {

  using MaterialHandle = std::string;

  template <typename Derived = Material>
  using MutableMaterialRef = Derived*;

  template <typename Derived = Material>
  using MaterialRef = const Derived*;

  class MaterialPool {
  public:
    ~MaterialPool() {
      for (auto& material : m_materials)
        delete material.second;
    }

    template <typename Derived, typename... Args>
    MutableMaterialRef<Derived> make(MaterialHandle handle, Args... args) {
      auto material = new Derived(args...);
      m_materials.emplace(handle, material);

      return ((Derived*)m_materials.at(handle));
    }

    template <typename Derived = Material>
    MutableMaterialRef<Derived> make(MaterialHandle handle, Material* material) {
      m_materials.emplace(handle, material);
      return m_materials.at(handle);
    }

    template <typename Derived = Material>
    std::optional<MaterialRef<Derived>> get(MaterialHandle handle) const {
      if (m_materials.contains(handle))
        return ((Derived*)m_materials.at(handle));

      return {};
    }

  private:
    std::unordered_map<MaterialHandle, Material*> m_materials;
  };

} // namespace Oxy::Pools
