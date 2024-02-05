#pragma once

#include "quixotism_c.hpp"
#include "vector.hpp"

namespace quixotism {

class Mat2 {
 public:
  using Column = Vec2;
  Column columns[2];

  Mat2() : columns{Column(1, 0), Column(0, 1)} {}
  Mat2(r32 val) : columns{Column(val, 0), Column(0, val)} {}

  Column &operator[](size_t idx) { return columns[idx]; }
  const Column &operator[](size_t idx) const { return columns[idx]; }

  static Mat2 Identity() { return Mat2(1.0); }

  Mat2 T() {
    std::swap(columns[0][1], columns[1][0]);
    return *this;
  }

  Mat2 Dot(const Mat2 &b) const {
    Mat2 result;
    result[0] = columns[0] * b[0][0] + columns[1] * b[0][1];
    result[1] = columns[0] * b[1][0] + columns[1] * b[1][1];
    return result;
  }

  r32 *DataPtr() { return columns[0].DataPtr(); }
};

inline Mat2 operator*(const Mat2 &a, const Mat2 &b) { return a.Dot(b); }

class Mat3 {
 public:
  using Column = Vec3;
  Column columns[3];

  Mat3() : columns{Column(1, 0, 0), Column(0, 1, 0), Column(0, 0, 1)} {}
  Mat3(r32 val)
      : columns{Column(val, 0, 0), Column(0, val, 0), Column(0, 0, val)} {}

  Column &operator[](size_t idx) { return columns[idx]; }
  const Column &operator[](size_t idx) const { return columns[idx]; }

  static Mat3 Identity() { return Mat3(1.0); }

  Mat3 T() {
    std::swap(columns[0][1], columns[1][0]);
    std::swap(columns[0][2], columns[2][0]);
    std::swap(columns[1][2], columns[2][1]);
    return *this;
  }

  Mat3 Dot(const Mat3 &b) const {
    Mat3 result;
    result[0] =
        columns[0] * b[0][0] + columns[1] * b[0][1] + columns[2] * b[0][2];
    result[1] =
        columns[0] * b[1][0] + columns[1] * b[1][1] + columns[2] * b[1][2];
    result[2] =
        columns[0] * b[2][0] + columns[1] * b[2][1] + columns[2] * b[2][2];
    ;
    return result;
  }

  r32 *DataPtr() { return columns[0].DataPtr(); }
};

inline Mat3 operator*(const Mat3 &a, const Mat3 &b) { return a.Dot(b); }

class Mat4 {
 public:
  using Column = Vec4;
  Column columns[4];

  Mat4()
      : columns{Column(1, 0, 0, 0), Column(0, 1, 0, 0), Column(0, 0, 1, 0),
                Column(0, 0, 0, 1)} {}
  Mat4(r32 val)
      : columns{Column(val, 0, 0, 0), Column(0, val, 0, 0),
                Column(0, 0, val, 0), Column(0, 0, 0, val)} {}

  Column &operator[](size_t idx) { return columns[idx]; }
  const Column &operator[](size_t idx) const { return columns[idx]; }

  static Mat4 Identity() { return Mat4(1.0); }

  Mat4 T() {
    std::swap(columns[0][1], columns[1][0]);
    std::swap(columns[0][2], columns[2][0]);
    std::swap(columns[0][3], columns[3][0]);
    std::swap(columns[1][2], columns[2][1]);
    std::swap(columns[1][3], columns[3][1]);
    std::swap(columns[2][3], columns[3][2]);
    return *this;
  }

  Mat4 Dot(const Mat4 &b) const {
    Mat4 result;
    result[0] = columns[0] * b[0][0] + columns[1] * b[0][1] +
                columns[2] * b[0][2] + columns[3] * b[0][3];
    result[1] = columns[0] * b[1][0] + columns[1] * b[1][1] +
                columns[2] * b[1][2] + columns[3] * b[1][3];
    result[2] = columns[0] * b[2][0] + columns[1] * b[2][1] +
                columns[2] * b[2][2] + columns[3] * b[2][3];
    result[3] = columns[0] * b[3][0] + columns[1] * b[3][1] +
                columns[2] * b[3][2] + columns[3] * b[3][3];
    return result;
  }

  r32 *DataPtr() { return columns[0].DataPtr(); }
};

inline Mat4 operator*(const Mat4 &a, const Mat4 &b) { return a.Dot(b); }

}  // namespace quixotism