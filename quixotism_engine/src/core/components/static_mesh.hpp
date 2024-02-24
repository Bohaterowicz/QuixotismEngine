#pragma once

#include "core/components/component.hpp"
#include "core/static_mesh_manager.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class StaticMeshComponent : public Component {
 public:
  StaticMeshComponent() = default;
  StaticMeshComponent(StaticMeshId id) : id(id) {}
  static ComponentType Type() { return ComponentType::STATIC_MESH; }

 private:
  StaticMeshId id;
};

}  // namespace quixotism
