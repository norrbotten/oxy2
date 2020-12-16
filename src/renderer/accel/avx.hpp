#pragma once

#include <immintrin.h>

#include "renderer/primitives/triangle.hpp"

#include "renderer/types.hpp"

namespace Oxy::Accel {

  static const __m256 one_m256       = _mm256_set1_ps(1.0f);
  static const __m256 minus_one_m256 = _mm256_set1_ps(-1.0f);
  static const __m256 pos_eps_m256   = _mm256_set1_ps(1e-6f);
  static const __m256 neg_eps_m256   = _mm256_set1_ps(-1e-6f);
  static const __m256 zero_m256      = _mm256_set1_ps(0.0f);
  static const __m256 fmax_m256      = _mm256_set1_ps(std::numeric_limits<float>::max());

  struct AVX2PackedTriangles {
    __m256 edge1[3];
    __m256 edge2[3];
    __m256 vert0[3];
    __m256 inactive;
  };

  struct AVX2SingleRay {
    __m256 origin[3];
    __m256 direction[3];
  };

  inline AVX2PackedTriangles
  get_packed_tris(const std::vector<Primitive::TrianglePrimitive>& triangles, size_t left,
                  size_t right) {

    assert((right - left) <= 8);

    AVX2PackedTriangles ptris;

    Vec3  edge1[8];
    Vec3  edge2[8];
    Vec3  vert0[8];
    float inactive[8];

    for (int n = 0; n < 8; n++) {
      if ((left + n) < right) {
        auto& tri = triangles.at(left + n);

        edge1[n]    = tri.p1() - tri.p0();
        edge2[n]    = tri.p2() - tri.p0();
        vert0[n]    = tri.p0();
        inactive[n] = 0.f;
      }
      else {
        edge1[n]    = Vec3();
        edge2[n]    = Vec3();
        vert0[n]    = Vec3();
        inactive[n] = 1.f;
      }
    }

    for (int n = 0; n < 3; n++) {
      ptris.edge1[n] = _mm256_set_ps(edge1[0][n], edge1[1][n], edge1[2][n], edge1[3][n],
                                     edge1[4][n], edge1[5][n], edge1[6][n], edge1[7][n]);

      ptris.edge2[n] = _mm256_set_ps(edge2[0][n], edge2[1][n], edge2[2][n], edge2[3][n],
                                     edge2[4][n], edge2[5][n], edge2[6][n], edge2[7][n]);

      ptris.vert0[n] = _mm256_set_ps(vert0[0][n], vert0[1][n], vert0[2][n], vert0[3][n],
                                     vert0[4][n], vert0[5][n], vert0[6][n], vert0[7][n]);
    }

    ptris.inactive = _mm256_set_ps(inactive[0], inactive[1], inactive[2], inactive[3], inactive[4],
                                   inactive[5], inactive[6], inactive[7]);

    return ptris;
  }

  inline AVX2SingleRay get_packed_ray(const SingleRay& ray) {
    AVX2SingleRay pray;

    for (int n = 0; n < 3; n++) {
      pray.origin[n]    = _mm256_set1_ps(ray.origin[n]);
      pray.direction[n] = _mm256_set1_ps(ray.direction[n]);
    }

    return pray;
  }

  inline void avx2_cross(__m256 result[3], const __m256 a[3], const __m256 b[3]) {
    result[0] = _mm256_fmsub_ps(a[1], b[2], _mm256_mul_ps(b[1], a[2]));
    result[1] = _mm256_fmsub_ps(a[2], b[0], _mm256_mul_ps(b[2], a[0]));
    result[2] = _mm256_fmsub_ps(a[0], b[1], _mm256_mul_ps(b[0], a[1]));
  }

  inline __m256 avx2_dot(const __m256 a[3], const __m256 b[3]) {
    return _mm256_fmadd_ps(a[2], b[2], _mm256_fmadd_ps(a[1], b[1], _mm256_mul_ps(a[0], b[0])));
  }

  inline void avx2_sub(__m256 result[3], const __m256 a[3], const __m256 b[3]) {
    result[0] = _mm256_sub_ps(a[0], b[0]);
    result[1] = _mm256_sub_ps(a[1], b[1]);
    result[2] = _mm256_sub_ps(a[2], b[2]);
  }

  inline float avx2_extract(const __m256 x, const int n) { return ((float*)(&x))[n]; }

  struct AVX2TriangleIntersectResult {
    float  t;
    size_t index;
  };

  inline AVX2TriangleIntersectResult avx2_ray_triangle_intersect(const AVX2PackedTriangles& tris,
                                                                 const AVX2SingleRay&       ray) {

    // q = cross(ray.dir, v0v2)
    __m256 q[3];
    avx2_cross(q, ray.direction, tris.edge2);

    // a = dot(v0v1, q)
    __m256 a = avx2_dot(tris.edge1, q);

    // f = 1 / a
    __m256 f = _mm256_div_ps(one_m256, a);

    // s = ray.origin - v0
    __m256 s[3];
    avx2_sub(s, ray.origin, tris.vert0);

    // u = f * dot(s, q)
    __m256 u = _mm256_mul_ps(f, avx2_dot(s, q));

    // r = cross(s, edge1)
    __m256 r[3];
    avx2_cross(r, s, tris.edge1);

    // v = f * dot(ray.dir, r)
    __m256 v = _mm256_mul_ps(f, avx2_dot(ray.direction, r));

    // t = f * dot(v0v2, r)
    __m256 t = _mm256_mul_ps(f, avx2_dot(tris.edge2, r));

    // mask of failed intersections
    __m256 failed;

    // t > eps && t < -eps
    failed = _mm256_and_ps(_mm256_cmp_ps(a, neg_eps_m256, _CMP_GT_OQ),
                           _mm256_cmp_ps(a, pos_eps_m256, _CMP_LT_OQ));

    // u > 0
    failed = _mm256_or_ps(failed, _mm256_cmp_ps(u, zero_m256, _CMP_LT_OQ));

    // v > 0
    failed = _mm256_or_ps(failed, _mm256_cmp_ps(v, zero_m256, _CMP_LT_OQ));

    // (u + v) < 1
    failed = _mm256_or_ps(failed, _mm256_cmp_ps(_mm256_add_ps(u, v), one_m256, _CMP_GT_OQ));

    // tri.inactive == false
    failed = _mm256_or_ps(failed, tris.inactive);

    // set failed ones to -1
    __m256 t_results = _mm256_blendv_ps(t, minus_one_m256, failed);

    // get failed isect mask as bitset in an int
    int mask = _mm256_movemask_ps(t_results);

    float  min   = std::numeric_limits<float>::max();
    size_t index = -1;

    float vals[8];
    _mm256_store_ps(vals, t_results);

    if (mask != 0xFF) {
      for (int n = 0; n < 8; n++) {
        if (vals[n] > 0.f && vals[n] < min) {
          min   = vals[n];
          index = 7 - n; // avx2 is stored in the reverse order, so 7 - idx to get the correct value
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
