#pragma once

#include "core/components/component.hpp"
#include "core/constants.hpp"
#include "math/math.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class TransformComponent : public Component {
 public:
  TransformComponent() : position{Vec3{0}}, scale{Vec3{1}}, rotation{Vec3{0}} {}

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

  Mat4 GetOrientationMatrix() const {
    Mat4 pos{1.0};
    pos[3] = Vec4{-position, 1.0};

    auto forward = Forward();
    auto right = Right();
    auto local_up = LocalUp();

    auto rot = Mat4(1.0F);
    rot[0] = Vec4(right, 0.0F);
    rot[1] = Vec4(local_up, 0.0F);
    rot[2] = Vec4(-forward, 0.0F);

    return rot.T() * pos;
  }

  void SetPosition(const Vec3 &new_pos) { position = new_pos; }

  static ComponentType Type() { return ComponentType::TRANSFORM; }

  Vec3 position;
  Vec3 scale;
  Vec3 rotation;  // rad
};

}  // namespace quixotism
