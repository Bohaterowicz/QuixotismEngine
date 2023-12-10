#pragma once

#include "matrix.hpp"
#include "quixotism_c.hpp"
#include "vector.hpp"

namespace quixotism {

inline r32 Dot(Vec2 &a, Vec2 &b) { return a.Dot(b); }
inline r32 Dot(Vec3 &a, Vec3 &b) { return a.Dot(b); }
inline r32 Dot(Vec4 &a, Vec4 &b) { return a.Dot(b); }

inline Vec2 Dot(Vec2 &a, Mat2 &b) { return a.Dot(b); }
inline Vec3 Dot(Vec3 &a, Mat3 &b) { return a.Dot(b); }
inline Vec4 Dot(Vec4 &a, Mat4 &b) { return a.Dot(b); }

inline Mat2 Dot(Mat2 &a, Mat2 &b) { return a * b; }
inline Mat3 Dot(Mat3 &a, Mat3 &b) { return a * b; }
inline Mat4 Dot(Mat4 &a, Mat4 &b) { return a * b; }

inline Vec3 Cross(const Vec3 &a, const Vec3 &b) {
  Vec3 Result(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x);
  return Result;
}

}  // namespace quixotism
