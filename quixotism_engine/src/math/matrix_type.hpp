#pragma once

#include "quixotism_c.hpp"
#include "vector_type.hpp"

namespace quixotism {

class Matrix {
 public:
 protected:
  Matrix() = default;

 private:
};

class Mat2 : public Matrix {
 public:
  using Column = Vec2;
  Column columns[2];

  constexpr Mat2() : columns{Column(1, 0), Column(0, 1)} {}
  constexpr Mat2(r32 val) : columns{Column(val, 0), Column(0, val)} {}

  constexpr Column &operator[](size_t idx) { return columns[idx]; }
  constexpr const Column &operator[](size_t idx) const { return columns[idx]; }

  static Mat2 Identity() { return Mat2(1.0); }

  Mat2 T() {
    std::swap(columns[0][1], columns[1][0]);
    return *this;
  }

  r32 *DataPtr() { return columns[0].DataPtr(); }
  const r32 *DataPtr() const { return columns[0].DataPtr(); }
};

class Mat3 : public Matrix {
 public:
  using Column = Vec3;
  Column columns[3];

  constexpr Mat3()
      : columns{Column(1, 0, 0), Column(0, 1, 0), Column(0, 0, 1)} {}
  constexpr Mat3(r32 val)
      : columns{Column(val, 0, 0), Column(0, val, 0), Column(0, 0, val)} {}

  constexpr Column &operator[](size_t idx) { return columns[idx]; }
  constexpr const Column &operator[](size_t idx) const { return columns[idx]; }

  static Mat3 Identity() { return Mat3(1.0); }

  Mat3 T() {
    std::swap(columns[0][1], columns[1][0]);
    std::swap(columns[0][2], columns[2][0]);
    std::swap(columns[1][2], columns[2][1]);
    return *this;
  }

  r32 *DataPtr() { return columns[0].DataPtr(); }
  const r32 *DataPtr() const { return columns[0].DataPtr(); }
};

class Mat4 : public Matrix {
 public:
  using Column = Vec4;
  Column columns[4];

  constexpr Mat4()
      : columns{Column(1, 0, 0, 0), Column(0, 1, 0, 0), Column(0, 0, 1, 0),
                Column(0, 0, 0, 1)} {}
  constexpr Mat4(r32 val)
      : columns{Column(val, 0, 0, 0), Column(0, val, 0, 0),
                Column(0, 0, val, 0), Column(0, 0, 0, val)} {}

  constexpr Column &operator[](size_t idx) { return columns[idx]; }
  constexpr const Column &operator[](size_t idx) const { return columns[idx]; }

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

  r32 *DataPtr() { return columns[0].DataPtr(); }
  const r32 *DataPtr() const { return columns[0].DataPtr(); }
};

template <typename T>
concept MatrixType = std::is_base_of_v<Matrix, T>;

}  // namespace quixotism