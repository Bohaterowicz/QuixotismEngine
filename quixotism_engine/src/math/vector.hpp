#pragma once

#include <cmath>

#include "quixotism_c.hpp"
#include "util/concepts.hpp"

namespace quixotism {

class Mat2;
class Mat3;
class Mat4;

class Vector {
 public:
  using ScalarType = DefaultMathPrecisionType;

 protected:
  Vector() = default;

 private:
};

/**
 * @brief Vec2 implementations
 *
 */
class Vec2 : public Vector {
 private:
  template <int x, int y>
  struct Vec2SwizzleWrapper {
    // implicit conversion to Vec2
    operator Vec2() { return Vec2(arr[x], arr[y]); }
    // support assignments from Vec2
    Vec2SwizzleWrapper& operator=(const Vec2& vec) {
      arr[x] = vec.x;
      arr[y] = vec.y;
      return *this;
    }

   private:
    ScalarType arr[2];
  };

 public:
  union {
    struct {
      ScalarType x, y;
    };
    ScalarType e[2];
    Vec2SwizzleWrapper<0, 0> xx;
    Vec2SwizzleWrapper<0, 1> xy;
    Vec2SwizzleWrapper<1, 0> yx;
    Vec2SwizzleWrapper<1, 1> yy;
  };

  constexpr Vec2() : x{0}, y{0} {}
  constexpr Vec2(ScalarType v) : x{v}, y{v} {}
  constexpr Vec2(ScalarType _x, ScalarType _y) : x{_x}, y{_y} {}

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

  Vec2& operator*=(ScalarType const& scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  Vec2& operator/=(ScalarType const& scalar) {
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

  ScalarType& operator[](size_t idx) { return e[idx]; }
  const ScalarType& operator[](size_t idx) const { return e[idx]; }

  ScalarType Length() const { return sqrtf(x * x + y * y); }
  ScalarType LengthSqr() const { return x * x + y * y; }

  ScalarType Dot(Vec2 const& other) const { return x * other.x + y * other.y; }
  Vec2 Dot(Mat2 const& other) const;

  Vec2& Normalize();

  ScalarType* DataPtr() { return &e[0]; }
  const ScalarType* DataPtr() const { return &e[0]; }
};

/**
 * @brief Vec3 implementations
 *
 */
class Vec3 : public Vector {
 private:
  template <int x, int y>
  struct Vec2SwizzleWrapper {
    // implicit conversion to Vec2
    operator Vec2() { return Vec2(arr[x], arr[y]); }
    // support assignments from Vec2
    Vec2SwizzleWrapper& operator=(const Vec2& vec) {
      arr[x] = vec.x;
      arr[y] = vec.y;
      return *this;
    }

   private:
    ScalarType arr[3];
  };

  template <int x, int y, int z>
  struct Vec3SwizzleWrapper {
    // implicit conversion to Vec3
    operator Vec3() { return Vec3(arr[x], arr[y], arr[z]); }
    // support assignments from Vec3
    Vec3SwizzleWrapper& operator=(const Vec3& vec) {
      arr[x] = vec.x;
      arr[y] = vec.y;
      arr[z] = vec.z;
      return *this;
    }

   private:
    ScalarType arr[3];
  };

 public:
  union {
    struct {
      ScalarType x, y, z;
    };
    struct {
      ScalarType pitch, yaw, roll;
    };
    ScalarType e[3];
    Vec2SwizzleWrapper<0, 0> xx;
    Vec2SwizzleWrapper<0, 1> xy;
    Vec2SwizzleWrapper<0, 2> xz;
    Vec2SwizzleWrapper<1, 0> yx;
    Vec2SwizzleWrapper<1, 1> yy;
    Vec2SwizzleWrapper<1, 2> yz;
    Vec2SwizzleWrapper<2, 0> zx;
    Vec2SwizzleWrapper<2, 1> zy;
    Vec2SwizzleWrapper<2, 2> zz;

    Vec3SwizzleWrapper<0, 0, 0> xxx;
    Vec3SwizzleWrapper<0, 0, 1> xxy;
    Vec3SwizzleWrapper<0, 0, 2> xxz;
    Vec3SwizzleWrapper<0, 1, 0> xyx;
    Vec3SwizzleWrapper<0, 1, 1> xyy;
    Vec3SwizzleWrapper<0, 1, 2> xyz;
    Vec3SwizzleWrapper<0, 2, 0> xzx;
    Vec3SwizzleWrapper<0, 2, 1> xzy;
    Vec3SwizzleWrapper<0, 2, 2> xzz;
    Vec3SwizzleWrapper<1, 0, 0> yxx;
    Vec3SwizzleWrapper<1, 0, 1> yxy;
    Vec3SwizzleWrapper<1, 0, 2> yxz;
    Vec3SwizzleWrapper<1, 1, 0> yyx;
    Vec3SwizzleWrapper<1, 1, 1> yyy;
    Vec3SwizzleWrapper<1, 1, 2> yyz;
    Vec3SwizzleWrapper<1, 2, 0> yzx;
    Vec3SwizzleWrapper<1, 2, 1> yzy;
    Vec3SwizzleWrapper<1, 2, 2> yzz;
    Vec3SwizzleWrapper<2, 0, 0> zxx;
    Vec3SwizzleWrapper<2, 0, 1> zxy;
    Vec3SwizzleWrapper<2, 0, 2> zxz;
    Vec3SwizzleWrapper<2, 1, 0> zyx;
    Vec3SwizzleWrapper<2, 1, 1> zyy;
    Vec3SwizzleWrapper<2, 1, 2> zyz;
    Vec3SwizzleWrapper<2, 2, 0> zzx;
    Vec3SwizzleWrapper<2, 2, 1> zzy;
    Vec3SwizzleWrapper<2, 2, 2> zzz;
  };

  constexpr Vec3() : x{0}, y{0}, z{0} {}
  constexpr Vec3(ScalarType v) : x{v}, y{v}, z{v} {}
  constexpr Vec3(ScalarType _x, ScalarType _y, ScalarType _z)
      : x{_x}, y{_y}, z{_z} {}
  constexpr Vec3(Vec2 v2, ScalarType _z) : x{v2.x}, y{v2.y}, z{_z} {}

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

  Vec3& operator*=(ScalarType const& scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  Vec3& operator/=(ScalarType const& scalar) {
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

  ScalarType& operator[](size_t idx) { return e[idx]; }
  const ScalarType& operator[](size_t idx) const { return e[idx]; }

  ScalarType Length() const { return sqrtf(x * x + y * y + z * z); }
  ScalarType LengthSqr() const { return x * x + y * y + z * z; }

  ScalarType Dot(Vec3 const& other) const {
    return x * other.x + y * other.y + z * other.z;
  }
  Vec3 Dot(Mat3 const& other) const;

  Vec3& Normalize();

  ScalarType* DataPtr() { return &e[0]; }
  const ScalarType* DataPtr() const { return &e[0]; }
};

/**
 * @brief Vec4 implementations
 *
 */
class Vec4 : public Vector {
 private:
  template <int x, int y>
  struct Vec2SwizzleWrapper {
    // implicit conversion to Vec2
    operator Vec2() { return Vec2(arr[x], arr[y]); }
    // support assignments from Vec2
    Vec2SwizzleWrapper& operator=(const Vec2& vec) {
      arr[x] = vec.x;
      arr[y] = vec.y;
      return *this;
    }

   private:
    ScalarType arr[4];
  };

  template <int x, int y, int z>
  struct Vec3SwizzleWrapper {
    // implicit conversion to Vec3
    operator Vec3() { return Vec3(arr[x], arr[y], arr[z]); }
    // support assignments from Vec3
    Vec3SwizzleWrapper& operator=(const Vec3& vec) {
      arr[x] = vec.x;
      arr[y] = vec.y;
      arr[z] = vec.z;
      return *this;
    }

   private:
    ScalarType arr[4];
  };

  template <int x, int y, int z, int w>
  struct Vec4SwizzleWrapper {
    // implicit conversion to Vec4
    operator Vec4() { return Vec4(arr[x], arr[y], arr[z], arr[w]); }
    // support assignments from Vec4
    Vec4SwizzleWrapper& operator=(const Vec4& vec) {
      arr[x] = vec.x;
      arr[y] = vec.y;
      arr[z] = vec.z;
      arr[w] = vec.w;
      return *this;
    }

   private:
    ScalarType arr[4];
  };

 public:
  union {
    struct {
      ScalarType x, y, z, w;
    };
    ScalarType e[4];

    Vec2SwizzleWrapper<0, 0> xx;
    Vec2SwizzleWrapper<0, 1> xy;
    Vec2SwizzleWrapper<0, 2> xz;
    Vec2SwizzleWrapper<0, 3> xw;
    Vec2SwizzleWrapper<1, 0> yx;
    Vec2SwizzleWrapper<1, 1> yy;
    Vec2SwizzleWrapper<1, 2> yz;
    Vec2SwizzleWrapper<1, 3> yw;
    Vec2SwizzleWrapper<2, 0> zx;
    Vec2SwizzleWrapper<2, 1> zy;
    Vec2SwizzleWrapper<2, 2> zz;
    Vec2SwizzleWrapper<2, 3> zw;
    Vec2SwizzleWrapper<3, 0> wx;
    Vec2SwizzleWrapper<3, 1> wy;
    Vec2SwizzleWrapper<3, 2> wz;
    Vec2SwizzleWrapper<3, 3> ww;

    Vec3SwizzleWrapper<0, 0, 0> xxx;
    Vec3SwizzleWrapper<0, 0, 1> xxy;
    Vec3SwizzleWrapper<0, 0, 2> xxz;
    Vec3SwizzleWrapper<0, 0, 3> xxw;
    Vec3SwizzleWrapper<0, 1, 0> xyx;
    Vec3SwizzleWrapper<0, 1, 1> xyy;
    Vec3SwizzleWrapper<0, 1, 2> xyz;
    Vec3SwizzleWrapper<0, 1, 3> xyw;
    Vec3SwizzleWrapper<0, 2, 0> xzx;
    Vec3SwizzleWrapper<0, 2, 1> xzy;
    Vec3SwizzleWrapper<0, 2, 2> xzz;
    Vec3SwizzleWrapper<0, 2, 3> xzw;
    Vec3SwizzleWrapper<0, 3, 0> xwx;
    Vec3SwizzleWrapper<0, 3, 1> xwy;
    Vec3SwizzleWrapper<0, 3, 2> xwz;
    Vec3SwizzleWrapper<0, 3, 3> xww;
    Vec3SwizzleWrapper<1, 0, 0> yxx;
    Vec3SwizzleWrapper<1, 0, 1> yxy;
    Vec3SwizzleWrapper<1, 0, 2> yxz;
    Vec3SwizzleWrapper<1, 0, 3> yxw;
    Vec3SwizzleWrapper<1, 1, 0> yyx;
    Vec3SwizzleWrapper<1, 1, 1> yyy;
    Vec3SwizzleWrapper<1, 1, 2> yyz;
    Vec3SwizzleWrapper<1, 1, 3> yyw;
    Vec3SwizzleWrapper<1, 2, 0> yzx;
    Vec3SwizzleWrapper<1, 2, 1> yzy;
    Vec3SwizzleWrapper<1, 2, 2> yzz;
    Vec3SwizzleWrapper<1, 2, 3> yzw;
    Vec3SwizzleWrapper<1, 3, 0> ywx;
    Vec3SwizzleWrapper<1, 3, 1> ywy;
    Vec3SwizzleWrapper<1, 3, 2> ywz;
    Vec3SwizzleWrapper<1, 3, 3> yww;
    Vec3SwizzleWrapper<2, 0, 0> zxx;
    Vec3SwizzleWrapper<2, 0, 1> zxy;
    Vec3SwizzleWrapper<2, 0, 2> zxz;
    Vec3SwizzleWrapper<2, 0, 3> zxw;
    Vec3SwizzleWrapper<2, 1, 0> zyx;
    Vec3SwizzleWrapper<2, 1, 1> zyy;
    Vec3SwizzleWrapper<2, 1, 2> zyz;
    Vec3SwizzleWrapper<2, 1, 3> zyw;
    Vec3SwizzleWrapper<2, 2, 0> zzx;
    Vec3SwizzleWrapper<2, 2, 1> zzy;
    Vec3SwizzleWrapper<2, 2, 2> zzz;
    Vec3SwizzleWrapper<2, 2, 3> zzw;
    Vec3SwizzleWrapper<2, 3, 0> zwx;
    Vec3SwizzleWrapper<2, 3, 1> zwy;
    Vec3SwizzleWrapper<2, 3, 2> zwz;
    Vec3SwizzleWrapper<2, 3, 3> zww;
    Vec3SwizzleWrapper<3, 0, 0> wxx;
    Vec3SwizzleWrapper<3, 0, 1> wxy;
    Vec3SwizzleWrapper<3, 0, 2> wxz;
    Vec3SwizzleWrapper<3, 0, 3> wxw;
    Vec3SwizzleWrapper<3, 1, 0> wyx;
    Vec3SwizzleWrapper<3, 1, 1> wyy;
    Vec3SwizzleWrapper<3, 1, 2> wyz;
    Vec3SwizzleWrapper<3, 1, 3> wyw;
    Vec3SwizzleWrapper<3, 2, 0> wzx;
    Vec3SwizzleWrapper<3, 2, 1> wzy;
    Vec3SwizzleWrapper<3, 2, 2> wzz;
    Vec3SwizzleWrapper<3, 2, 3> wzw;
    Vec3SwizzleWrapper<3, 3, 0> wwx;
    Vec3SwizzleWrapper<3, 3, 1> wwy;
    Vec3SwizzleWrapper<3, 3, 2> wwz;
    Vec3SwizzleWrapper<3, 3, 3> www;
  };

  constexpr Vec4() : x{0}, y{0}, z{0}, w{0} {}
  constexpr Vec4(ScalarType v) : x{v}, y{v}, z{v}, w{v} {}
  constexpr Vec4(ScalarType _x, ScalarType _y, ScalarType _z, ScalarType _w)
      : x{_x}, y{_y}, z{_z}, w{_w} {}
  constexpr Vec4(Vec3 v3, ScalarType _w) : x{v3.x}, y{v3.y}, z{v3.z}, w{_w} {}

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

  Vec4& operator*=(ScalarType const& scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
  }

  Vec4& operator/=(ScalarType const& scalar) {
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

  ScalarType& operator[](size_t idx) { return e[idx]; }
  const ScalarType& operator[](size_t idx) const { return e[idx]; }

  ScalarType Length() const { return sqrtf(x * x + y * y + z * z + w * w); }
  ScalarType LengthSqr() const { return x * x + y * y + z * z + w * w; }

  ScalarType Dot(Vec4 const& other) const {
    return x * other.x + y * other.y + z * other.z + w * other.w;
  }
  Vec4 Dot(Mat4 const& other) const;

  Vec4& Normalize();

  ScalarType* DataPtr() { return &e[0]; }
  const ScalarType* DataPtr() const { return &e[0]; }
};

template <typename T>
concept VectorType = std::is_base_of_v<Vector, T>;

template <VectorType T>
inline T operator+(T a, T const& b) {
  return a += b;
}
template <VectorType T>
inline T operator-(T a, T const& b) {
  return a -= b;
}
template <VectorType T>
inline T operator*(T a, r32 const& s) {
  return a *= s;
}
template <VectorType T>
inline T operator*(r32 const& s, T a) {
  return a *= s;
}
template <VectorType T>
inline T operator/(T a, r32 const& s) {
  return a /= s;
}

template <VectorType T>
T Normalize(const T A) {
  T Result = A;
  return Result.Normalize();
}

template <VectorType T>
r32 Length(const T A) {
  return A.Length();
}

template <VectorType T>
r32 LengthSqr(const T A) {
  return A.LengthSqr();
}

}  // namespace quixotism