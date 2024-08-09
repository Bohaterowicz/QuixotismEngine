#pragma once

#include "matrix_type.hpp"
#include "quixotism_c.hpp"
#include "vector_type.hpp"

namespace quixotism {

inline r32 Dot(const Vec2 &a, const Vec2 &b) { return a.x * b.x + a.y * b.y; }

inline r32 operator*(const Vec2 &a, const Vec2 &b) { return Dot(a, b); }

inline r32 Dot(const Vec3 &a, const Vec3 &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline r32 operator*(const Vec3 &a, const Vec3 &b) { return Dot(a, b); }

inline r32 Dot(const Vec4 &a, const Vec4 &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline r32 operator*(const Vec4 &a, const Vec4 &b) { return Dot(a, b); }

inline Vec2 Dot(const Mat2 &m, const Vec2 &v) {
  Vec2 result;
  result.x = m[0].x * v.x + m[0].x * v.y;
  result.y = m[0].y * v.x + m[0].y * v.y;
  return result;
}

inline Vec2 Dot(const Vec2 &v, const Mat2 &m) { return Dot(m, v); }
inline Vec2 operator*(const Mat2 &m, const Vec2 &v) { return Dot(m, v); }
inline Vec2 operator*(const Vec2 &v, const Mat2 &m) { return Dot(m, v); }

inline Vec3 Dot(const Mat3 &m, const Vec3 &v) {
  Vec3 result;
  result.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z;
  result.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z;
  result.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z;
  return result;
}

inline Vec3 Dot(const Vec3 &v, const Mat3 &m) { return Dot(m, v); }
inline Vec3 operator*(const Mat3 &m, const Vec3 &v) { return Dot(m, v); }
inline Vec3 operator*(const Vec3 &v, const Mat3 &m) { return Dot(m, v); }

inline Vec4 Dot(const Mat4 &m, const Vec4 &v) {
  Vec4 result;
  /*
  result.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z + m[3].x * v.w;
  result.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z + m[3].y * v.w;
  result.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z + m[3].z * v.w;
  result.w = m[0].w * v.x + m[1].w * v.y + m[2].w * v.z + m[3].w * v.w;
  */
  auto xy = _mm256_set_m128(_mm_set1_ps(v.y), _mm_set1_ps(v.x));
  auto zw = _mm256_set_m128(_mm_set1_ps(v.w), _mm_set1_ps(v.z));
  auto c1c2 = _mm256_load_ps(&m[0].e[0]);
  auto c3c4 = _mm256_load_ps(&m[2].e[0]);
  auto fmadd = _mm256_add_ps(_mm256_mul_ps(c1c2, xy), _mm256_mul_ps(c3c4, zw));
  result.v = _mm_add_ps(_mm256_extractf128_ps(fmadd, 0),
                        _mm256_extractf128_ps(fmadd, 1));

  return result;
}

// Special optimized version of dot product which performs a Mat4*Vec4 dot
// product on 2 provided Vec4s utlizing wide AVX registers
inline void Dot_2Vec(const Mat4 &m, const Vec4 &v1, const Vec4 &v2,
                     Vec4 &out_v1, Vec4 &out_v2) {
  auto accum1 =
      _mm256_mul_ps(_mm256_set_m128(m[0].v, m[0].v),
                    _mm256_set_m128(_mm_set1_ps(v2.x), _mm_set1_ps(v1.x)));
  auto accum2 =
      _mm256_mul_ps(_mm256_set_m128(m[2].v, m[2].v),
                    _mm256_set_m128(_mm_set1_ps(v2.z), _mm_set1_ps(v1.z)));
  accum1 = _mm256_fmadd_ps(
      _mm256_set_m128(m[1].v, m[1].v),
      _mm256_set_m128(_mm_set1_ps(v2.y), _mm_set1_ps(v1.y)), accum1);
  accum2 = _mm256_fmadd_ps(
      _mm256_set_m128(m[3].v, m[3].v),
      _mm256_set_m128(_mm_set1_ps(v2.w), _mm_set1_ps(v1.w)), accum2);
  accum1 = _mm256_add_ps(accum1, accum2);
  out_v1.v = _mm256_extractf128_ps(accum1, 0);
  out_v2.v = _mm256_extractf128_ps(accum1, 1);
}

inline Vec4 Dot(const Vec4 &v, const Mat4 &m) { return Dot(m, v); }
inline Vec4 operator*(const Mat4 &m, const Vec4 &v) { return Dot(m, v); }
inline Vec4 operator*(const Vec4 &v, const Mat4 &m) { return Dot(m, v); }

inline r32 Cross(const Vec2 &a, const Vec2 &b) { return a.x * b.y - a.y * b.x; }

inline Vec3 Cross(const Vec3 &a, const Vec3 &b) {
  Vec3 Result(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x);
  return Result;
}

inline Mat2 Dot(const Mat2 &a, const Mat2 &b) {
  Mat2 result;
  result[0] = a[0] * b[0][0] + a[1] * b[0][1];
  result[1] = a[0] * b[1][0] + a[1] * b[1][1];
  return result;
}
inline Mat2 operator*(const Mat2 &a, const Mat2 &b) { return Dot(a, b); }

inline Mat3 Dot(const Mat3 &a, const Mat3 &b) {
  Mat3 result;
  result[0] = a[0] * b[0][0] + a[1] * b[0][1] + a[2] * b[0][2];
  result[1] = a[0] * b[1][0] + a[1] * b[1][1] + a[2] * b[1][2];
  result[2] = a[0] * b[2][0] + a[1] * b[2][1] + a[2] * b[2][2];
  return result;
}
inline Mat3 operator*(const Mat3 &a, const Mat3 &b) { return Dot(a, b); }

inline Mat4 Dot(const Mat4 &a, const Mat4 &b) {
  Mat4 result;
  /*
  Mat4 res2;
  result[0] = a[0] * b[0].x + a[1] * b[0].y + a[2] * b[0].z + a[3] * b[0].w;
  result[1] = a[0] * b[1].x + a[1] * b[1].y + a[2] * b[1].z + a[3] * b[1].w;
  result[2] = a[0] * b[2].x + a[1] * b[2].y + a[2] * b[2].z + a[3] * b[2].w;
  result[3] = a[0] * b[3].x + a[1] * b[3].y + a[2] * b[3].z + a[3] * b[3].w;
  */
  Dot_2Vec(a, b[0], b[1], result[0], result[1]);
  Dot_2Vec(a, b[2], b[3], result[2], result[3]);
  return result;
}
inline Mat4 operator*(const Mat4 &a, const Mat4 &b) { return Dot(a, b); }

}  // namespace quixotism
