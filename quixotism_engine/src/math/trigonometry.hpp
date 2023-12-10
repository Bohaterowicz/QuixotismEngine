#pragma once

#include <cmath>

#include "quixotism_c.hpp"

namespace quixotism {

static constexpr r32 PI32 = 3.14159265359;

inline r32 Sin(r32 Rad) {
  r32 result = sinf(Rad);
  return result;
}

inline r32 Cos(r32 Rad) {
  r32 result = cosf(Rad);
  return result;
}

inline r32 Tan(r32 Rad) {
  r32 result = tanf(Rad);
  return result;
}

inline r32 ATan(r32 Rad) {
  auto result = atanf(Rad);
  return result;
}

constexpr r32 DegToRad(r32 deg) {
  r32 result = deg * (PI32 / 180.0F);
  return result;
}

constexpr r32 RadToDeg(r32 rad) {
  r32 result = rad * (180.0F / PI32);
  return result;
}

}  // namespace quixotism
