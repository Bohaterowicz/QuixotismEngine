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
  }
}

Transform Terminal::DrawTransform() {
  Transform transform;
  transform.SetPosition(Vec3{0, (anim_progress * normalized_height), 0});
  return transform;
}

}  // namespace quixotism
