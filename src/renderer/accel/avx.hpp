#pragma once

#include <immintrin.h>

#include "renderer/primitives/triangle.hpp"

#include "renderer/types.hpp"

namespace Oxy::Accel {

  static const __m256d one_m256       = _mm256_set1_pd(1.0);
  static const __m256d minus_one_m256 = _mm256_set1_pd(-1.0);
  static const __m256d pos_eps_m256   = _mm256_set1_pd(1e-6);
  static const __m256d neg_eps_m256   = _mm256_set1_pd(-1e-6);
  static const __m256d zero_m256      = _mm256_set1_pd(0.0);
  static const __m256d fmax_m256      = _mm256_set1_pd(std::numeric_limits<double>::max());

  struct AVX2PackedTriangles {
    __m256d edge1[3];
    __m256d edge2[3];
    __m256d vert0[3];
    __m256d inactive;
  };

  struct AVX2SingleRay {
    __m256d origin[3];
    __m256d direction[3];
  };

  inline AVX2PackedTriangles
  get_packed_tris(const std::vector<Primitive::TrianglePrimitive>& triangles, size_t left,
                  size_t right) {

    assert((right - left) <= 8);

    AVX2PackedTriangles ptris;

    Vec3  edge1[4];
    Vec3  edge2[4];
    Vec3  vert0[4];
    float inactive[4];

    for (int n = 0; n < 4; n++) {
      if ((left + n) < right) {
        auto& tri = triangles.at(left + n);

        edge1[n]    = tri.p1() - tri.p0();
        edge2[n]    = tri.p2() - tri.p0();
        vert0[n]    = tri.p0();
        inactive[n] = 0.0;
      }
      else {
        edge1[n]    = Vec3();
        edge2[n]    = Vec3();
        vert0[n]    = Vec3();
        inactive[n] = 1.0;
      }
    }

    for (int n = 0; n < 3; n++) {
      /*
      ptris.edge1[n] = _mm256_set_ps(edge1[0][n], edge1[1][n], edge1[2][n], edge1[3][n],
                                     edge1[4][n], edge1[5][n], edge1[6][n], edge1[7][n]);

      ptris.edge2[n] = _mm256_set_ps(edge2[0][n], edge2[1][n], edge2[2][n], edge2[3][n],
                                     edge2[4][n], edge2[5][n], edge2[6][n], edge2[7][n]);

      ptris.vert0[n] = _mm256_set_ps(vert0[0][n], vert0[1][n], vert0[2][n], vert0[3][n],
                                     vert0[4][n], vert0[5][n], vert0[6][n], vert0[7][n]);
                                     */

      ptris.edge1[n] = _mm256_set_pd(edge1[0][n], edge1[1][n], edge1[2][n], edge1[3][n]);

      ptris.edge2[n] = _mm256_set_pd(edge2[0][n], edge2[1][n], edge2[2][n], edge2[3][n]);

      ptris.vert0[n] = _mm256_set_pd(vert0[0][n], vert0[1][n], vert0[2][n], vert0[3][n]);
    }

    // ptris.inactive = _mm256_set_ps(inactive[0], inactive[1], inactive[2], inactive[3],
    // inactive[4],
    //                               inactive[5], inactive[6], inactive[7]);

    ptris.inactive = _mm256_set_pd(inactive[0], inactive[1], inactive[2], inactive[3]);

    return ptris;
  }

  inline AVX2SingleRay get_packed_ray(const SingleRay& ray) {
    AVX2SingleRay pray;

    for (int n = 0; n < 3; n++) {
      pray.origin[n]    = _mm256_set1_pd(ray.origin[n]);
      pray.direction[n] = _mm256_set1_pd(ray.direction[n]);
    }

    return pray;
  }

  inline void avx2_cross(__m256d result[3], const __m256d a[3], const __m256d b[3]) {
    result[0] = _mm256_fmsub_pd(a[1], b[2], _mm256_mul_pd(b[1], a[2]));
    result[1] = _mm256_fmsub_pd(a[2], b[0], _mm256_mul_pd(b[2], a[0]));
    result[2] = _mm256_fmsub_pd(a[0], b[1], _mm256_mul_pd(b[0], a[1]));
  }

  inline __m256d avx2_dot(const __m256d a[3], const __m256d b[3]) {
    return _mm256_fmadd_pd(a[2], b[2], _mm256_fmadd_pd(a[1], b[1], _mm256_mul_pd(a[0], b[0])));
  }

  inline void avx2_sub(__m256d result[3], const __m256d a[3], const __m256d b[3]) {
    result[0] = _mm256_sub_pd(a[0], b[0]);
    result[1] = _mm256_sub_pd(a[1], b[1]);
    result[2] = _mm256_sub_pd(a[2], b[2]);
  }

  struct AVX2TriangleIntersectResult {
    double t;
    size_t index;
  };

  inline AVX2TriangleIntersectResult avx2_ray_triangle_intersect(const AVX2PackedTriangles& tris,
                                                                 const AVX2SingleRay&       ray) {

    // q = cross(ray.dir, v0v2)
    __m256d q[3];
    avx2_cross(q, ray.direction, tris.edge2);

    // a = dot(v0v1, q)
    __m256d a = avx2_dot(tris.edge1, q);

    // f = 1 / a
    __m256d f = _mm256_div_pd(one_m256, a);

    // s = ray.origin - v0
    __m256d s[3];
    avx2_sub(s, ray.origin, tris.vert0);

    // u = f * dot(s, q)
    __m256d u = _mm256_mul_pd(f, avx2_dot(s, q));

    // r = cross(s, edge1)
    __m256d r[3];
    avx2_cross(r, s, tris.edge1);

    // v = f * dot(ray.dir, r)
    __m256d v = _mm256_mul_pd(f, avx2_dot(ray.direction, r));

    // t = f * dot(v0v2, r)
    __m256d t = _mm256_mul_pd(f, avx2_dot(tris.edge2, r));

    // mask of failed intersections
    __m256d failed;

    // t > eps && t < -eps
    failed = _mm256_and_pd(_mm256_cmp_pd(a, neg_eps_m256, _CMP_GT_OQ),
                           _mm256_cmp_pd(a, pos_eps_m256, _CMP_LT_OQ));

    // u > 0
    failed = _mm256_or_pd(failed, _mm256_cmp_pd(u, zero_m256, _CMP_LT_OQ));

    // v > 0
    failed = _mm256_or_pd(failed, _mm256_cmp_pd(v, zero_m256, _CMP_LT_OQ));

    // (u + v) < 1
    failed = _mm256_or_pd(failed, _mm256_cmp_pd(_mm256_add_pd(u, v), one_m256, _CMP_GT_OQ));

    // tri.inactive == false
    failed = _mm256_or_pd(failed, tris.inactive);

    // set failed ones to -1
    __m256d t_results = _mm256_blendv_pd(t, minus_one_m256, failed);

    // get failed isect mask as bitset in an int
    int mask = _mm256_movemask_pd(t_results);

    double min   = std::numeric_limits<double>::max();
    size_t index = -1;

    double vals[4];
    _mm256_store_pd(vals, t_results);

    if (mask != 0xFF) {
      for (int n = 0; n < 4; n++) {
        if (vals[n] > 0.f && vals[n] < min) {
          min   = vals[n];
          index = 3 - n; // avx2 is stored in the reverse order, so 7 - idx to get the correct value
        }
      }
    }

    return {min, index};

    /*
    // find the minimum index
    __m256 v1    = _mm256_permute_ps(t_results, 0b10110001);
    __m256 v2    = _mm256_min_ps(t_results, v1);
    __m256 v3    = _mm256_permute_ps(v2, 0b01001110);
    __m256 v4    = _mm256_min_ps(v2, v3);
    __m256 v5    = _mm256_castpd_ps(_mm256_permute4x64_pd(_mm256_castps_pd(v4), 0b01001110));
    __m256 v_min = _mm256_min_ps(v4, v5);
    __m256 mask  = _mm256_cmp_ps(t_results, v_min, 0);

    auto index = _tzcnt_u32(_mm256_movemask_ps(mask));

    // get the value
    float vals[8];
    _mm256_store_ps(vals, t_results);

    return {vals[index], index};
    */
  }

} // namespace Oxy::Accel
