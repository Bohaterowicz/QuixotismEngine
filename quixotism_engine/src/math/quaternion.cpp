#include "quaternion.hpp"

#include "trigonometry.hpp"

namespace quixotism {

Quaternion Quaternion::FromEulerAngles(Vec3 eualer_angels) {
  r32 cos_roll = Cos(eualer_angels.roll * 0.5);
  r32 sin_roll = Sin(eualer_angels.roll * 0.5);
  r32 cos_pitch = Cos(eualer_angels.pitch * 0.5);
  r32 sin_pitch = Sin(eualer_angels.pitch * 0.5);
  r32 cos_yaw = Cos(eualer_angels.yaw * 0.5);
  r32 sin_yaw = Sin(eualer_angels.yaw * 0.5);

  Quaternion result;
  result.w = cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw;
  result.x = sin_roll * cos_pitch * cos_yaw -
             cos_roll * sin_pitch * sin_yaw;  // roll (cos of roll)
  result.y = cos_roll * cos_pitch * sin_yaw -
             sin_roll * sin_pitch * cos_yaw;  // yaw (cos of yaw)
  result.z = cos_roll * sin_pitch * cos_yaw +
             sin_roll * cos_pitch * sin_yaw;  // pitch (cos of pitch)

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
