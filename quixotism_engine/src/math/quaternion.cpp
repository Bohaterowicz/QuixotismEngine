#include "quaternion.hpp"

#include "trigonometry.hpp"

namespace quixotism {

Quaternion Quaternion::FromEulerAngles(Vec3 euler_angels) {
  r32 cos_roll = Cos(euler_angels.roll * 0.5);
  r32 sin_roll = Sin(euler_angels.roll * 0.5);
  r32 cos_pitch = Cos(euler_angels.pitch * 0.5);
  r32 sin_pitch = Sin(euler_angels.pitch * 0.5);
  r32 cos_yaw = Cos(euler_angels.yaw * 0.5);
  r32 sin_yaw = Sin(euler_angels.yaw * 0.5);

  /*
    IMPORTANT: We create quaternion in Yaw-Pitch-Roll convention, where yaw is
    rotation around y-axis, pitch rotation around z-axis and roll around x-axis.
    This is according to the engine convention where FORWARD is the positive
    x-axis
  */

  Quaternion result;
  result.w = cos_yaw * cos_pitch * cos_roll - sin_yaw * sin_pitch * sin_roll;
  result.x = cos_yaw * cos_pitch * sin_roll + sin_yaw * sin_pitch * cos_roll;
  result.y = sin_yaw * cos_pitch * cos_roll - cos_yaw * sin_pitch * sin_roll;
  result.z = cos_yaw * sin_pitch * cos_roll + sin_yaw * cos_pitch * sin_roll;

  return result;
}

Mat3 Quaternion::CreateRotationMatrix() const {
  Mat3 result;

  result[0].x = 1 - 2 * (y * y + z * z);
  result[0].y = 2 * (x * y + w * z);
  result[0].z = 2 * (x * z - w * y);

  result[1].x = 2 * (x * y - w * z);
  result[1].y = 1 - 2 * (x * x + z * z);
  result[1].z = 2 * (y * z + w * x);

  result[2].x = 2 * (x * z + w * y);
  result[2].y = 2 * (y * z - w * x);
  result[2].z = 1 - 2 * (x * x + y * y);

  return result;
}

Quaternion &Quaternion::Normalize() {
  auto magnitude = sqrtf(w * w + x * x + y * y + z * z);
  w /= magnitude;
  x /= magnitude;
  y /= magnitude;
  z /= magnitude;
  return *this;
}

}  // namespace quixotism
