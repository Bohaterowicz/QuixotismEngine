#pragma once

#include "core/material_manager.hpp"
#include "core/static_mesh_manager.hpp"
#include "core/transform.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class Terminal {
 public:
  enum class VisibilityState { HIDDEN = 0, IN_ANIM, VISIBILE, OUT_ANIM };
  Terminal() = default;

  void Init();

  void Update(r32 delta_t);

  bool IsVisible() const { return visibility_state != VisibilityState::HIDDEN; }
  bool ShouldShow() const { return should_show; }
  void Show(bool show) { should_show = show; }
  void ToggleShow() { Show(!should_show); }

  Transform DrawTransform();

  StaticMeshId smid = 0;
  MaterialID matid = 0;

 private:
  VisibilityState visibility_state = VisibilityState::HIDDEN;
  bool should_show = false;

  r32 ease_anim_speed = 1000.f;
  r32 anim_progress = 0.f;
  u32 height = 100;
  r32 normalized_height = 0;
};

}  // namespace quixotism
