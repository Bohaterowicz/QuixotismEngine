#pragma once

#include "quixotism_c.hpp"

namespace quixotism {

enum ComponentType {
  TRANSFORM,
  CAMERA,
  __COUNT__,
};

static constexpr u32 COMPONENT_COUNT = ComponentType::__COUNT__;

class Component {
 public:
  Component() = default;

  static ComponentType Type() = delete;
};

}  // namespace quixotism
