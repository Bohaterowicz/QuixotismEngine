#pragma once

#include <tuple>

#include "concepts.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

template <Iterable T>
constexpr auto Enumerate(T &&iterable) {
  using TIter = decltype(std::begin(std::declval<T>()));
  struct iterator {
    size_t i;
    TIter iter;
    bool operator!=(const iterator &other) const { return iter != other.iter; }
    void operator++() {
      ++i;
      ++iter;
    }
    auto operator*() const { return std::tie(i, *iter); }
  };
  struct iterable_wrapper {
    T &&iterable;
    auto begin() { return iterator{0, std::begin(iterable)}; }
    auto end() { return iterator{0, std::end(iterable)}; }
  };
  return iterable_wrapper{std::forward<T>(iterable)};
}

}  // namespace quixotism
