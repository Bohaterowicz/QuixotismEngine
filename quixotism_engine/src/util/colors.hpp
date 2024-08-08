#pragma once

#include "math/vector_type.hpp"

namespace quixotism {

class Color {
 public:
  static constexpr Vec3 WHITE = Vec3{1.0, 1.0, 1.0};
  static constexpr Vec3 BLACK = Vec3{0.0, 0.0, 0.0};
  static constexpr Vec3 RED = Vec3{1.0, 0.0, 0.0};
  static constexpr Vec3 GREEN = Vec3{0.0, 1.0, 0.0};
  static constexpr Vec3 BLUE = Vec3{0.0, 0.0, 1.0};
  static constexpr Vec3 YELLOW = Vec3{1.0, 1.0, 0.0};
};

}  // namespace quixotism
