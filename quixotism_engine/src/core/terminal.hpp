#pragma once

#include <string>
#include <vector>

#include "colors.hpp"
#include "core/material_manager.hpp"
#include "core/static_mesh_manager.hpp"
#include "core/text_input.hpp"
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

  void PushLog(std::string &txt, Vec3 color = Color::RED) {
    text_lines.emplace_back(txt, color);
    if (text_lines.size() > 9) {
      text_lines.erase(text_lines.begin());
    }
  }

  Transform DrawTransform();

  StaticMeshId smid = 0;
  MaterialID matid = 0;

 private:
  struct TextInfo {
    std::string text;
    Vec3 color;
  };

  VisibilityState visibility_state = VisibilityState::HIDDEN;
  bool should_show = false;

  void CommitInput(std::string txt);

  TextInput input;

  std::vector<TextInfo> text_lines;

  r32 ease_anim_speed = 1000.f;
  r32 anim_progress = 0.f;
  u32 height = 200;
  r32 normalized_height = 0;
};

}  // namespace quixotism
