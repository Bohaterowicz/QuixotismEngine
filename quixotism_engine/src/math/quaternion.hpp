#pragma once

#include "matrix.hpp"

namespace quixotism {

class Quaternion {
 public:
  Quaternion() : w{1}, x{0}, y{0}, z{0} {}

  Mat3 CreateRotationMatrix() const;

  Quaternion &Normalize();

  static Quaternion FromEulerAngles(Vec3 euler_angles);

 private:
  r32 w, x, y, z;
};

}  // namespace quixotism
