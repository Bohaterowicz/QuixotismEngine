#pragma once
#include "util/concepts.hpp"

namespace quixotism {
template <Numeric T>
T Min(T a, T b) {
  return (a < b) ? a : b;
}

template <Numeric T>
T Max(T a, T b) {
  return (a > b) ? a : b;
}

template <Numeric T>
T Abs(T a) {
  return (a < 0) ? -a : a;
}
}  // namespace quixotism