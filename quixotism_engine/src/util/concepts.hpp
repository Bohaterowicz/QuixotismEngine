#pragma once

#include <type_traits>

namespace quixotism {

template <class T>
concept Numeric = std::is_arithmetic_v<T>;

}  // namespace quixotism
