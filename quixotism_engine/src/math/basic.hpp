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
}  // namespace quixotism