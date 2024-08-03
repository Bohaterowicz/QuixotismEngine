#pragma once

#include <ranges>
#include <type_traits>

namespace quixotism {

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <typename T>
concept Iterable = std::ranges::range<T>;

}  // namespace quixotism
