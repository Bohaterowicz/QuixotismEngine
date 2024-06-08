#pragma once

#include "core/transform.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

enum ComponentType {
  TRANSFORM,
  CAMERA,
  STATIC_MESH,
  __COUNT__,
};

static constexpr u32 COMPONENT_COUNT = ComponentType::__COUNT__;

class Component {
 public:
  Component() = default;

  static ComponentType Type() = delete;

  Transform transform;
};

}  // namespace quixotism
