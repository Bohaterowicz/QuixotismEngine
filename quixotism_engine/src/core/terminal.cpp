#include "core/terminal.hpp"

#include <vector>

#include "core/quixotism_engine.hpp"
#include "math/qmath.hpp"
#include "renderer/quixotism_renderer.hpp"

namespace quixotism {

void Terminal::Init() {
  auto &engine = QuixotismEngine::GetEngine();
  auto dim = engine.GetWindowDim();
  normalized_height = ((r32)height / (r32)dim.height) * 2;
  std::vector<Vec3> axes_verts{Vec3{-1, 1 + normalized_height, 0},
                               Vec3{-1, 1, 0},
                               Vec3{1, 1 + normalized_height, 0},
                               Vec3{-1, 1, 0},
                               Vec3{1, 1, 0},
                               Vec3{1, 1 + normalized_height, 0}};
  Mesh m;
  m.VertexPosData = axes_verts;
  StaticMesh smesh{std::move(m)};
  smid = engine.static_mesh_mgr.Add(std::move(smesh));
  VertexArrayID vao_id = VertexArray::INVALID_VAO_ID;
  VertexBufferLayout layout;
  layout.AddLayoutElementF(3, false, 0);
  if (auto new_vao =
          QuixotismRenderer::GetRenderer().vertex_array_mgr.Create(layout)) {
    vao_id = *new_vao;
  } else {
    Assert(0);
  }
  QuixotismRenderer::GetRenderer().MakeDrawableStaticMesh2(smid, vao_id);

  matid = engine.material_mgr.Add(Material{
      QuixotismRenderer::GetRenderer().shader_mgr.GetByName("solid_color")});

  text_lines = {"1. Hello Terminal",
                "2. Some commands:",
                "3. COMMAND_SOMETHING",
                "4. COMMAND_SOMETHING_ELSE",
                "5. MAYBE SOME OTHER COMMAND",
                "6. EXIT",
                "7. HACK MATRIX",
                "8. ACTIVATE SKYNET",
                "9. POWER"};

  auto commit_func =
      std::bind(&Terminal::CommitInput, this, std::placeholders::_1);
  input.Init(commit_func);
}

void Terminal::CommitInput(std::string txt) {
  DBG_PRINT(txt);
  text_lines.erase(text_lines.begin());
  text_lines.push_back(txt);
}

void Terminal::Update(r32 delta_t) {
  if (should_show) {
    if (visibility_state == VisibilityState::HIDDEN) {
      anim_progress = 0;
      // terminal should show, but is hidden -> start showing terminal
      visibility_state = VisibilityState::IN_ANIM;
      anim_progress += (delta_t * ease_anim_speed) / height;
    } else if (visibility_state == VisibilityState::IN_ANIM) {
      if (anim_progress >= 1.0) {
        visibility_state = VisibilityState::VISIBILE;
      } else {
        anim_progress += (delta_t * ease_anim_speed) / height;
        if (anim_progress > 1.0) {
          anim_progress = 1.0;
        }
      }
    }
    input.SetFocus(true);
  } else {
    if (visibility_state == VisibilityState::VISIBILE ||
        visibility_state == VisibilityState::IN_ANIM) {
      visibility_state = VisibilityState::OUT_ANIM;
      anim_progress -= (delta_t * ease_anim_speed) / height;
    } else if (visibility_state == VisibilityState::OUT_ANIM) {
      if (anim_progress <= 0.0) {
        visibility_state = VisibilityState::HIDDEN;
        anim_progress = 0;
      } else {
        anim_progress -= (delta_t * ease_anim_speed) / height;
        if (anim_progress <= 0.0) {
          anim_progress = 0.0;
        }
      }
    }
    input.SetFocus(false);
  }

  if (visibility_state != VisibilityState::HIDDEN) {
    auto &engine = QuixotismEngine::GetEngine();
    auto dim = engine.GetWindowDim();
    auto line_height = (r32)height / 10;
    auto normalized_line_height = ((r32)line_height / (r32)dim.height) * 2;
    auto scale = engine.font.px_scale * line_height;
    auto x = -0.99;
    auto y = (1.0 - normalized_line_height) + normalized_height -
             (normalized_height * anim_progress);
    for (auto &txt : text_lines) {
      engine.DrawText(txt, x, y, scale, 1);
      y -= normalized_line_height;
    }
    input.Update(y, scale);
  }
}

Transform Terminal::DrawTransform() {
  Transform transform;
  transform.SetPosition(Vec3{0, (anim_progress * normalized_height), 0});
  return transform;
}

}  // namespace quixotism
