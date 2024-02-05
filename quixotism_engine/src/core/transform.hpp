#pragma once

#include "math/math.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class Transform {
 public:
  Transform() : position{Vec3{0}}, scale{Vec3{1}}, rotation{Vec3{0}} {}
  inline static Vec3 FORWARD{1, 0, 0};
  inline static Vec3 RIGHT{0, 0, 1};
  inline static Vec3 UP{0, 1, 0};

  Vec3 Forward() const {
    auto rotation_quat = Quaternion::FromEulerAngles(rotation);
    return Normalize(FORWARD * rotation_quat.CreateRotationMatrix());
  }

  Vec3 Right() const {
    auto forward = Forward();
    return Normalize(Cross(forward, UP));
  }

  Vec3 LocalUp() const {
    auto forward = Forward();
    auto right = Right();
    return Normalize(Cross(right, forward));
  }

  Vec3 position;
  Vec3 scale;
  Vec3 rotation;  // rad
};

}  // namespace quixotism
