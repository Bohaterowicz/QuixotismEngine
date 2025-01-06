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
  constexpr Mat2(Column c0, Column c1) : columns{c0, c1} {}

  constexpr Column &operator[](size_t idx) { return columns[idx]; }
  constexpr const Column &operator[](size_t idx) const { return columns[idx]; }

  static Mat2 Identity() { return Mat2(1.0); }

  Mat2 T() {
    std::swap(columns[0][1], columns[1][0]);
    return *this;
  }

  r32 Determinant() const {
    return columns[0].x * columns[1].y - columns[0].y * columns[1].x;
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
  constexpr Mat3(Column c0, Column c1, Column c2) : columns{c0, c1, c2} {}

  constexpr Column &operator[](size_t idx) { return columns[idx]; }
  constexpr const Column &operator[](size_t idx) const { return columns[idx]; }

  static Mat3 Identity() { return Mat3(1.0); }

  Mat3 T() {
    std::swap(columns[0][1], columns[1][0]);
    std::swap(columns[0][2], columns[2][0]);
    std::swap(columns[1][2], columns[2][1]);
    return *this;
  }

  r32 Determinant() const {
    return (columns[0].x *
            Mat2{Vec2{columns[1].yz}, Vec2{columns[2].yz}}.Determinant()) -
           (columns[1].x *
            Mat2{Vec2{columns[0].yz}, Vec2{columns[2].yz}}.Determinant()) +
           (columns[2].x *
            Mat2{Vec2{columns[0].yz}, Vec2{columns[1].yz}}.Determinant());
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

  Mat4 &T() {
    std::swap(columns[0][1], columns[1][0]);
    std::swap(columns[0][2], columns[2][0]);
    std::swap(columns[0][3], columns[3][0]);
    std::swap(columns[1][2], columns[2][1]);
    std::swap(columns[1][3], columns[3][1]);
    std::swap(columns[2][3], columns[3][2]);
    return *this;
  }

  r32 Determinant() const {
    return (columns[0].x * Mat3(Vec3{columns[1].yzw}, Vec3{columns[2].yzw},
                                Vec3{columns[3].yzw})
                               .Determinant()) -
           (columns[1].x * Mat3(Vec3{columns[0].yzw}, Vec3{columns[2].yzw},
                                Vec3{columns[3].yzw})
                               .Determinant()) +
           (columns[2].x * Mat3(Vec3{columns[0].yzw}, Vec3{columns[1].yzw},
                                Vec3{columns[3].yzw})
                               .Determinant()) -
           (columns[3].x * Mat3(Vec3{columns[0].yzw}, Vec3{columns[1].yzw},
                                Vec3{columns[2].yzw})
                               .Determinant());
  }

  Mat4 Inverse() const {
    auto det = Determinant();
    Assert(det != 0);  // if det==0, matrix is not invertible
    auto inv_det = 1 / det;

    Mat4 result;

    result[0][0] =
        Mat3{Vec3{columns[1].yzw}, Vec3{columns[2].yzw}, Vec3{columns[3].yzw}}
            .Determinant() *
        inv_det;
    result[1][0] =
        -(Mat3{Vec3{columns[1].xzw}, Vec3{columns[2].xzw}, Vec3{columns[3].xzw}}
              .Determinant() *
          inv_det);
    result[2][0] =
        Mat3{Vec3{columns[1].xyw}, Vec3{columns[2].xyw}, Vec3{columns[3].xyw}}
            .Determinant() *
        inv_det;
    result[3][0] =
        -(Mat3{Vec3{columns[1].xyz}, Vec3{columns[2].xyz}, Vec3{columns[3].xyz}}
              .Determinant() *
          inv_det);

    result[0][1] =
        -(Mat3{Vec3{columns[0].yzw}, Vec3{columns[2].yzw}, Vec3{columns[3].yzw}}
              .Determinant() *
          inv_det);
    result[1][1] =
        Mat3{Vec3{columns[0].xzw}, Vec3{columns[2].xzw}, Vec3{columns[3].xzw}}
            .Determinant() *
        inv_det;
    result[2][1] =
        -(Mat3{Vec3{columns[0].xyw}, Vec3{columns[2].xyw}, Vec3{columns[3].xyw}}
              .Determinant() *
          inv_det);
    result[3][1] =
        Mat3{Vec3{columns[0].xyz}, Vec3{columns[2].xyz}, Vec3{columns[3].xyz}}
            .Determinant() *
        inv_det;

    result[0][2] =
        Mat3{Vec3{columns[0].yzw}, Vec3{columns[1].yzw}, Vec3{columns[3].yzw}}
            .Determinant() *
        inv_det;
    result[1][2] =
        -(Mat3{Vec3{columns[0].xzw}, Vec3{columns[1].xzw}, Vec3{columns[3].xzw}}
              .Determinant() *
          inv_det);
    result[2][2] =
        Mat3{Vec3{columns[0].xyw}, Vec3{columns[1].xyw}, Vec3{columns[3].xyw}}
            .Determinant() *
        inv_det;
    result[3][2] =
        -(Mat3{Vec3{columns[0].xyz}, Vec3{columns[1].xyz}, Vec3{columns[3].xyz}}
              .Determinant() *
          inv_det);

    result[0][3] =
        -(Mat3{Vec3{columns[0].yzw}, Vec3{columns[1].yzw}, Vec3{columns[2].yzw}}
              .Determinant() *
          inv_det);
    result[1][3] =
        Mat3{Vec3{columns[0].xzw}, Vec3{columns[1].xzw}, Vec3{columns[2].xzw}}
            .Determinant() *
        inv_det;
    result[2][3] =
        -(Mat3{Vec3{columns[0].xyw}, Vec3{columns[1].xyw}, Vec3{columns[2].xyw}}
              .Determinant() *
          inv_det);
    result[3][3] =
        Mat3{Vec3{columns[0].xyz}, Vec3{columns[1].xyz}, Vec3{columns[2].xyz}}
            .Determinant() *
        inv_det;

    return result;
  }

  r32 *DataPtr() { return columns[0].DataPtr(); }
  const r32 *DataPtr() const { return columns[0].DataPtr(); }
};

template <typename T>
concept MatrixType = std::is_base_of_v<Matrix, T>;

}  // namespace quixotism