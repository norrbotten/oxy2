#pragma once

#include <mutex>

#include "renderer/macros.hpp"
#include "renderer/types.hpp"

#include "renderer/material/color.hpp"

namespace Oxy::Integrators::Photonmapping {

  /*
    The lightmap is an octree type data structure, but data isnt only stored in the leaf nodes.
    In progressive light mapping, we dont just sample with luxels (or whatever its called) of a
    constant radius, but from very large to very small ones. The first node of the lightmap is
    essentially the average color of the entire map, which each split halving its radius and
    doubling the resolution.

    I have no idea what this even really implements, i slapped this together in an afternoon
  */

  class ProgressiveLightmapNode {
  public:
    ~ProgressiveLightmapNode() {
      if (m_children != nullptr)
        delete[] m_children;
    }

    [[nodiscard]] auto lock() { return std::lock_guard{m_branch_mtx}; }

    void add_sample(Color energy) {
      auto g{lock()};

      m_energy += energy;
      m_num_samples++;
    }

    Color energy() const {
      if (m_num_samples == 0)
        return Color();

      return m_energy / m_num_samples;
    }

    void subdivide_children() {
      auto g{lock()};
      m_children = new ProgressiveLightmapNode[8]();
    }

    CREF(children);
    REF(children);

  private:
    std::mutex m_branch_mtx;

    Color        m_energy{0.0, 0.0, 0.0};
    unsigned int m_num_samples{0};

    ProgressiveLightmapNode* m_children{nullptr};
  };

  class ProgressiveLightmap {
  public:
    ProgressiveLightmap(FloatType smallest_photon, FloatType lightmap_radius,
                        Vec3 origin = {0, 0, 0})
        : m_smallest_photon(smallest_photon)
        , m_lightmap_radius(lightmap_radius)
        , m_origin(origin)
        , m_root(new ProgressiveLightmapNode()) {}

    ~ProgressiveLightmap() {
      if (m_root != nullptr)
        delete m_root;
    }

    // Averages samples in a branch of the lightmap where radius is between min and max
    Color sample(Vec3 position, FloatType min_sample_radius, FloatType max_sample_radius) const {
      assert(min_sample_radius > m_smallest_photon);

      const auto* node = m_root; // this a const context

      auto radius = m_lightmap_radius;
      auto pos    = position - m_origin;

      Color result{};
      int   num_samples = 0;

      // this came to me in a dream
      while (radius > min_sample_radius) {
        if (node->children() == nullptr)
          break;

        result += node->energy();
        num_samples++;

        // select which subnode to go to
        int idx = int(pos.x > 0) + 2 * int(pos.y > 0) + 4 * int(pos.z > 0);
        node    = node->children() + idx;

        // halve our space, and offset our position, so we end up in the right node next time
        radius = radius / 2;
        pos += radius * Vec3(pos.x > 0 ? -1 : 1, pos.y > 0 ? -1 : 1, pos.z > 0 ? -1 : 1);
      }

      if (node->children() != nullptr) {
        // some kind of filtering based on distance here
        for (int i = 0; i < 8; i++)
          result += (node->children() + i)->energy() * 8;

        num_samples += 8;
      }

      if (num_samples == 0) // shouldnt really happen
        return Color{};

      return result / num_samples;
    }

    void add_sample(Vec3 position, Color energy, FloatType photon_radius) {
      assert(photon_radius > m_smallest_photon);

      auto node   = m_root;
      auto radius = m_lightmap_radius;
      auto pos    = position - m_origin;

      // same traversal algo as above
      while (radius > photon_radius) {
        if (node->children() == nullptr)
          node->subdivide_children();

        // select which subnode to go to
        int idx = int(pos.x > 0) + 2 * int(pos.y > 0) + 4 * int(pos.z > 0);
        node    = node->children() + idx;

        // halve our space, and offset our position, so we end up in the right node next time
        radius = radius / 2;
        pos += radius * Vec3(pos.x > 0 ? -1 : 1, pos.y > 0 ? -1 : 1, pos.z > 0 ? -1 : 1);
      }

      assert(node != nullptr);

      node->add_sample(energy);
    }

  private:
    FloatType m_smallest_photon;
    FloatType m_lightmap_radius;

    Vec3 m_origin;

    ProgressiveLightmapNode* m_root;
  };

} // namespace Oxy::Integrators::Photonmapping
