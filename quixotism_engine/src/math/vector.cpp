#include "vector.hpp"

#include "matrix.hpp"

namespace quixotism {

Vec2 &Vec2::Normalize() {
  auto len = Length();
  return *this /= len;
}

Vec3 &Vec3::Normalize() {
  auto len = Length();
  return *this /= len;
}

Vec4 &Vec4::Normalize() {
  auto len = Length();
  return *this /= len;
}

Vec2 Vec2::Dot(Mat2 const &other) const {
  Vec2 result;
  result.x = other[0].x * x + other[0].y * y;
  result.y = other[1].x * x + other[1].y * y;

  return result;
}

Vec3 Vec3::Dot(Mat3 const &other) const {
  Vec3 result;
  result.x = other[0].x * x + other[0].y * y + other[0].z * z;
  result.y = other[1].x * x + other[1].y * y + other[1].z * z;
  result.z = other[2].x * x + other[2].y * y + other[2].z * z;

  return result;
}

Vec4 Vec4::Dot(Mat4 const &other) const {
  Vec4 result;
  result.x = other[0].x * x + other[0].y * y + other[0].z * z + other[0].w * w;
  result.y = other[1].x * x + other[1].y * y + other[1].z * z + other[1].w * w;
  result.z = other[2].x * x + other[2].y * y + other[2].z * z + other[2].w * w;
  result.w = other[3].x * x + other[3].y * y + other[3].z * z + other[3].w * w;

  return result;
}

}  // namespace quixotism
