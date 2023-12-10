#pragma once

#include <cmath>

#include "quixotism_c.hpp"

namespace quixotism {

class Mat2;
class Mat3;
class Mat4;

/**
 * @brief Vec2 implementations
 *
 */
class Vec2 {
 public:
  union {
    struct {
      r32 x, y;
    };
    r32 e[2];
  };

  Vec2() : x{0}, y{0} {}
  Vec2(r32 v) : x{v}, y{v} {}
  Vec2(r32 _x, r32 _y) : x{_x}, y{_y} {}

  Vec2& operator+=(Vec2 const& other) {
    x += other.x;
    y += other.y;
    return *this;
  }
  Vec2& operator-=(Vec2 const& other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Vec2& operator*=(r32 const& scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  Vec2& operator/=(r32 const& scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
  }

  Vec2 operator-() const {
    Vec2 result;
    result.x = -x;
    result.y = -y;
    return result;
  }

  r32& operator[](size_t idx) { return e[idx]; }
  const r32& operator[](size_t idx) const { return e[idx]; }

  r32 Length() const { return sqrtf(x * x + y * y); }
  r32 LengthSqr() const { return x * x + y * y; }

  r32 Dot(Vec2 const& other) const { return x * other.x + y * other.y; }
  Vec2 Dot(Mat2 const& other) const;

  Vec2& Normalize();

  r32* DataPtr() { return &e[0]; }
};

inline Vec2 operator+(Vec2 a, Vec2 const& b) { return a += b; }
inline Vec2 operator-(Vec2 a, Vec2 const& b) { return a -= b; }
inline Vec2 operator*(Vec2 a, r32 const& s) { return a *= s; }
inline Vec2 operator*(Vec2 a, Mat2 const& m) { return a.Dot(m); }
inline Vec2 operator/(Vec2 a, r32 const& s) { return a /= s; }

class IVec2 {
 public:
  union {
    struct {
      i32 x, y;
    };
    i32 e[2];
  };

  IVec2() : x{0}, y{0} {}
  IVec2(i32 v) : x{v}, y{v} {}
  IVec2(i32 _x, i32 _y) : x{_x}, y{_y} {}

  IVec2& operator+=(IVec2 const& other) {
    x += other.x;
    y += other.y;
    return *this;
  }
  IVec2& operator-=(IVec2 const& other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  IVec2 operator-() const {
    IVec2 result;
    result.x = -x;
    result.y = -y;
    return result;
  }

  i32& operator[](size_t idx) { return e[idx]; }
  const i32& operator[](size_t idx) const { return e[idx]; }

  r32 Length() const { return sqrtf(x * x + y * y); }
  r32 LengthSqr() const { return x * x + y * y; }
  r32 Dot(IVec2 const& other) const { return x * other.x + y * other.y; }

  i32* DataPtr() { return &e[0]; }
};

inline IVec2 operator+(IVec2 a, IVec2 const& b) { return a += b; }
inline IVec2 operator-(IVec2 a, IVec2 const& b) { return a -= b; }

/**
 * @brief Vec3 implementations
 *
 */
class Vec3 {
 public:
  union {
    struct {
      r32 x, y, z;
    };
    struct {
      // Vec3 is also used sometimes to represent rotations, Sice in our
      // convention the default engine forward direction that all objects face
      // by default is the +x-axis (1, 0, 0), and the up direction is the y
      // axis, thus the right vector i the +z axis, this means that roll is
      // around the x, yaw around y and pitch around z
      r32 roll, yaw, pitch;
    };
    r32 e[3];
  };

  Vec3() : x{0}, y{0}, z{0} {}
  Vec3(r32 v) : x{v}, y{v}, z{v} {}
  Vec3(r32 _x, r32 _y, r32 _z) : x{_x}, y{_y}, z{_z} {}
  Vec3(Vec2 v2, r32 _z) : x{v2.x}, y{v2.y}, z{_z} {}

  Vec3& operator+=(Vec3 const& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }
  Vec3& operator-=(Vec3 const& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  Vec3& operator*=(r32 const& scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  Vec3& operator/=(r32 const& scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
  }

  Vec3 operator-() const {
    Vec3 result;
    result.x = -x;
    result.y = -y;
    result.z = -z;
    return result;
  }

  r32& operator[](size_t idx) { return e[idx]; }
  const r32& operator[](size_t idx) const { return e[idx]; }

  r32 Length() const { return sqrtf(x * x + y * y + z * z); }
  r32 LengthSqr() const { return x * x + y * y + z * z; }

  r32 Dot(Vec3 const& other) const {
    return x * other.x + y * other.y + z * other.z;
  }
  Vec3 Dot(Mat3 const& other) const;

  Vec3& Normalize();

  r32* DataPtr() { return &e[0]; }
};

inline Vec3 operator+(Vec3 a, Vec3 const& b) { return a += b; }
inline Vec3 operator-(Vec3 a, Vec3 const& b) { return a -= b; }
inline Vec3 operator*(Vec3 a, r32 const& s) { return a *= s; }
inline Vec3 operator*(Vec3 a, Mat3 const& m) { return a.Dot(m); }
inline Vec3 operator/(Vec3 a, r32 const& s) { return a /= s; }

class IVec3 {};

/**
 * @brief Vec4 implementations
 *
 */
class Vec4 {
 public:
  union {
    struct {
      r32 x, y, z, w;
    };
    r32 e[4];
  };

  Vec4() : x{0}, y{0}, z{0}, w{0} {}
  Vec4(r32 v) : x{v}, y{v}, z{v}, w{v} {}
  Vec4(r32 _x, r32 _y, r32 _z, r32 _w) : x{_x}, y{_y}, z{_z}, w{_w} {}
  Vec4(Vec3 v3, r32 _w) : x{v3.x}, y{v3.y}, z{v3.z}, w{_w} {}

  Vec4& operator+=(Vec4 const& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
  }

  Vec4& operator-=(Vec4 const& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
  }

  Vec4& operator*=(r32 const& scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
  }

  Vec4& operator/=(r32 const& scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
  }

  Vec4 operator-() const {
    Vec4 result;
    result.x = -x;
    result.y = -y;
    result.z = -z;
    result.w = -w;
    return result;
  }

  r32& operator[](size_t idx) { return e[idx]; }
  const r32& operator[](size_t idx) const { return e[idx]; }

  r32 Length() const { return sqrtf(x * x + y * y + z * z + w * w); }
  r32 LengthSqr() const { return x * x + y * y + z * z + w * w; }

  r32 Dot(Vec4 const& other) const {
    return x * other.x + y * other.y + z * other.z + w * other.w;
  }
  Vec4 Dot(Mat4 const& other) const;

  Vec4& Normalize();

  r32* DataPtr() { return &e[0]; }
};

inline Vec4 operator+(Vec4 a, Vec4 const& b) { return a += b; }
inline Vec4 operator-(Vec4 a, Vec4 const& b) { return a -= b; }
inline Vec4 operator*(Vec4 a, r32 const& s) { return a *= s; }
inline Vec4 operator*(Vec4 a, Mat4 const& m) { return a.Dot(m); }
inline Vec4 operator/(Vec4 a, r32 const& s) { return a /= s; }

class IVec4 {};

template <class T>
T Normalize(const T A) {
  return A.Normalize();
}

}  // namespace quixotism