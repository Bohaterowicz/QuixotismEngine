#pragma once

#include "quixotism_c.hpp"
#include "transform.hpp"

namespace quixotism {

class Entity {
 public:
  Entity() = default;

  Transform& GetTransform() { return transform; }
  void SetPosition(const Vec3 pos) { transform.position = pos; }

 protected:
  Transform transform;

 private:
};

}  // namespace quixotism
