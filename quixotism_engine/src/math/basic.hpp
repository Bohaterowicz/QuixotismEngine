#pragma once

#include "quixotism_c.hpp"
#include "util/concepts.hpp"

namespace quixotism {

inline constexpr r32 qepsilon = 1e-6;

template <Numeric T>
constexpr T Min(T a, T b) {
  return (a < b) ? a : b;
}

template <Numeric T>
constexpr T Max(T a, T b) {
  return (a > b) ? a : b;
}

template <Numeric T>
constexpr T Abs(T a) {
  return (a < 0) ? -a : a;
}
}  // namespace quixotism