#pragma once

#include "core/components/component.hpp"
#include "core/material_manager.hpp"
#include "core/static_mesh_manager.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class StaticMeshComponent : public Component {
 public:
  StaticMeshComponent() = default;
  StaticMeshComponent(StaticMeshId _sm_id, MaterialID _mat_id)
      : sm_id{_sm_id}, mat_id{_mat_id} {}
  static ComponentType Type() { return ComponentType::STATIC_MESH; }

  [[no_discard]] StaticMeshId GetStaticMeshId() const { return sm_id; }
  [[no_discard]] MaterialID GetMaterialID() const { return mat_id; }

 private:
  StaticMeshId sm_id;
  MaterialID mat_id;
};

}  // namespace quixotism
