#pragma once

#include <type_traits>

#include "renderer/primitives/plane.hpp"
#include "renderer/primitives/traits.hpp"

#include "renderer/accel/avx.hpp"

namespace Oxy::Accel {

  inline bool ray_vs_aabb(const Vec3& orig, const Vec3& dir, const Vec3& vmin, const Vec3& vmax,
                          FloatType& t) {

    auto rdx = 1.0 / dir.x;
    auto rdy = 1.0 / dir.y;
    auto rdz = 1.0 / dir.z;

    auto t1 = (vmin.x - orig.x) * rdx;
    auto t2 = (vmax.x - orig.x) * rdx;
    auto t3 = (vmin.y - orig.y) * rdy;
    auto t4 = (vmax.y - orig.y) * rdy;
    auto t5 = (vmin.z - orig.z) * rdz;
    auto t6 = (vmax.z - orig.z) * rdz;

    auto t7 = std::max(std::min(t1, t2), std::max(std::min(t3, t4), std::min(t5, t6)));
    auto t8 = std::min(std::max(t1, t2), std::min(std::max(t3, t4), std::max(t5, t6)));

    if (t8 < 0.0 || t8 < t7)
      return false;

    t = t7;

    return true;
  }

  template <typename T>
  struct BVHNode {
    BVHNode(std::vector<T>& prims, std::vector<AVX2PackedTriangles>& ptris)
        : primitives(prims)
        , packed_triangles(ptris) {
      // cant build bvh of planes, they dont have a valid bbox
      static_assert(!std::is_same<T, Primitive::PlanePrimitive>::value);
    }

    ~BVHNode() {
      if (left_node != nullptr)
        delete left_node;

      if (right_node != nullptr)
        delete right_node;
    }

    std::vector<T>& primitives;

    std::vector<AVX2PackedTriangles>& packed_triangles;
    size_t                            packed_triangles_index;

    BoundingBox bbox;

    BVHNode* left_node  = nullptr;
    BVHNode* right_node = nullptr;

    size_t left_index, right_index;
  };

  template <typename T>
  BoundingBox get_bbox(const std::vector<T>& primitives, size_t start, size_t end) {
    Vec3 min(std::numeric_limits<FloatType>::max());
    Vec3 max(std::numeric_limits<FloatType>::lowest());

    for (auto it = primitives.begin() + start; it != primitives.begin() + end; it++) {
      auto [bbox_min, bbox_max] = Primitive::Traits::bbox(*it);

      for (int axis = 0; axis < 3; axis++) {
        min[axis] = std::min(min[axis], bbox_min[axis]);
        max[axis] = std::max(max[axis], bbox_max[axis]);
      }
    }

    return {min, max};
  }

  template <typename T>
  BVHNode<T>* build_bvh(std::vector<T>& primitives, std::vector<AVX2PackedTriangles>& ptris,
                        size_t left, size_t right) {

    auto* node = new BVHNode<T>(primitives, ptris);

    node->left_index  = left;
    node->right_index = right;

    auto [min_bbox, max_bbox] = get_bbox<T>(primitives, left, right);

    node->bbox = {min_bbox, max_bbox};

    if (right - left <= 8) { // 8 or less primitives in each leaf
      node->packed_triangles_index = node->packed_triangles.size();
      node->packed_triangles.push_back(get_packed_tris(primitives, left, right));

      return node;
    }

    const auto begin = primitives.begin() + left;
    const auto end   = primitives.begin() + right;

    // find the longest axis
    int    longest_axis = -1;
    double longest_len  = 0;

    for (int i = 0; i < 3; i++) {
      auto len = max_bbox[i] - min_bbox[i];

      if (len > longest_len) {
        longest_len  = len;
        longest_axis = i;
      }
    }

    assert(longest_axis != -1);

    // sort along the longest axis
    std::sort(begin, end, [longest_axis](auto prim1, auto prim2) {
      return Primitive::Traits::midpoint(prim1)[longest_axis] <
             Primitive::Traits::midpoint(prim2)[longest_axis];
    });

    auto middle = (left + right) / 2;

    node->left_node  = build_bvh<T>(primitives, ptris, left, middle);
    node->right_node = build_bvh<T>(primitives, ptris, middle, right);

    return node;
  }

  struct BVHTraverseResult {
    bool      hit   = false;
    FloatType t     = std::numeric_limits<FloatType>::max();
    size_t    index = -1;
  };

  template <typename T>
  BVHTraverseResult traverse_bvh(BVHNode<T>* bvh, const SingleRay& ray) {
    BVHTraverseResult res;

    auto pray = get_packed_ray(ray);

    static thread_local BVHNode<T>* stack[1024] = {0};
    int                             stack_ptr   = 0;

    stack[stack_ptr++] = bvh;

    while (stack_ptr > 0) {
      auto node = stack[--stack_ptr];

      FloatType dummy;
      if (ray_vs_aabb(ray.origin, ray.direction, node->bbox.min, node->bbox.max, dummy)) {
        bool is_leaf = (node->left_node == nullptr) && (node->right_node == nullptr);

        if (is_leaf) {
          // auto   beg = node->primitives.begin();
          // size_t idx = 0;

          auto avx_res = avx2_ray_triangle_intersect(
              node->packed_triangles.at(node->packed_triangles_index), pray);

          if (avx_res.index != (size_t)-1 && avx_res.t < res.t) {
            res.hit   = true;
            res.t     = avx_res.t;
            res.index = node->left_index + avx_res.index;
          }

          /*
          for (auto it = beg + node->left_index; it != beg + node->right_index; it++) {
            if (auto dist = Primitive::Traits::intersect_ray(*it, ray); dist.has_value()) {
              if (dist.value() < res.t) {
                res.hit   = true;
                res.t     = dist.value();
                res.index = node->left_index + idx;
              }
            }

            idx++;
          }
          */
        }
        else {
          if (node->left_node != nullptr)
            stack[stack_ptr++] = node->left_node;

          if (node->right_node != nullptr)
            stack[stack_ptr++] = node->right_node;
        }
      }
    }

    return res;
  }

} // namespace Oxy::Accel
