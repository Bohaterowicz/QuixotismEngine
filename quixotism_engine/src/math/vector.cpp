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
  result.x = other[0].x * x + other[0].x * y;
  result.y = other[0].y * x + other[0].y * y;

  return result;
}

Vec3 Vec3::Dot(Mat3 const &other) const {
  Vec3 result;
  result.x = other[0].x * x + other[1].x * y + other[2].x * z;
  result.y = other[0].y * x + other[1].y * y + other[2].y * z;
  result.z = other[0].z * x + other[1].z * y + other[2].z * z;

  return result;
}

Vec4 Vec4::Dot(Mat4 const &other) const {
  Vec4 result;
  result.x = other[0].x * x + other[1].x * y + other[2].x * z + other[3].x * w;
  result.y = other[0].y * x + other[1].y * y + other[2].y * z + other[3].y * w;
  result.z = other[0].z * x + other[1].z * y + other[2].z * z + other[3].z * w;
  result.w = other[0].w * x + other[1].w * y + other[2].w * z + other[3].w * w;

  return result;
}

}  // namespace quixotism
