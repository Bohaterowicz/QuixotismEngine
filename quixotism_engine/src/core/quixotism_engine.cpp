#include "quixotism_engine.hpp"

#include "core/constants.hpp"
#include "dbg_print.hpp"
#include "file_processing/obj_parser/obj_parser.hpp"
#include "renderer/quixotism_renderer.hpp"

namespace quixotism {

void QuixotismEngine::Init(const PlatformServices& init_services,
                           const WindowDim& dim) {
  services = init_services;
  window_dim = dim;

  Entity camera;
  CameraComponent cam_com{
      DegToRad(45.0),
      static_cast<r32>(window_dim.width) / static_cast<r32>(window_dim.height),
      0.01f, 1000.0f};
  camera.AddComponent(cam_com);

  camera_id = entity_mgr.Add(std::move(camera));
  entity_mgr.Get(camera_id)->transform.SetPosition(Vec3{-150, 0, 50});

  InitTextFonts();

  auto obj_data = QuixotismEngine::GetEngine().services.read_file(
      "D:/QuixotismEngine/quixotism_engine/data/meshes/box.obj");
  auto meshes = ParseOBJ(obj_data.data.get(), obj_data.size);

  StaticMesh smesh{std::move(meshes[0])};
  auto mesh_id = static_mesh_mgr.Add(std::move(smesh));
  QuixotismRenderer::GetRenderer().MakeDrawableStaticMesh(mesh_id);

  Entity box;
  StaticMeshComponent box_sm{mesh_id};
  box.AddComponent(box_sm);
  box_id = entity_mgr.Add(std::move(box));
  auto box_id2 = entity_mgr.Clone(box_id);
  entity_mgr.Get(box_id2)->transform.Move(Vec3{100, 100, 100});
}

void QuixotismEngine::UpdateAndRender(ControllerInput& input, r32 delta_t) {
  auto& renderer = QuixotismRenderer::GetRenderer();

  auto& transform = entity_mgr.Get(camera_id)->transform;

  auto speed = 50.0F;  // m/s
  auto rotation_speed = 1.0F;
  auto movement = Vec3{0.0F};

  if (input.up.ended_down) {
    movement += UP;
  }
  if (input.down.ended_down) {
    movement += -UP;
  }
  if (input.forward.ended_down) {
    movement += transform.Forward();
  }
  if (input.backward.ended_down) {
    movement += -transform.Forward();
  }
  if (input.right.ended_down) {
    movement += transform.Right();
  }
  if (input.left.ended_down) {
    movement += -transform.Right();
  }
  if (movement.Length() > 0.0F) {
    transform.position += (movement.Normalize() * speed * delta_t);
  }

  auto rotation_delta = Vec3{input.mouse_y_delta, input.mouse_x_delta, 0} *
                        rotation_speed * delta_t;

  if (rotation_delta.LengthSqr() > 0.0F) {
    transform.rotation += rotation_delta;
    if (transform.rotation.pitch > DegToRad(85)) {
      auto rotation_adjustment = Vec3{};
      auto adjustment = DegToRad(85) - transform.rotation.pitch;
      if (adjustment < DegToRad(-0.01)) {
        rotation_adjustment.pitch = adjustment;
        transform.rotation += rotation_adjustment;
      } else {
        transform.rotation = Vec3{DegToRad(85), transform.rotation.yaw, 0.0};
      }
    } else if (transform.rotation.pitch < DegToRad(-85)) {
      auto rotation_adjustment = Vec3{};
      auto adjustment = DegToRad(-85) + transform.rotation.pitch;
      if (adjustment > DegToRad(0.01)) {
        rotation_adjustment.pitch = adjustment;
        transform.rotation += rotation_adjustment;
      } else {
        transform.rotation = Vec3{DegToRad(-85), transform.rotation.yaw, 0.0};
      }
    }
  }

  renderer.ClearRenderTarget();
  DrawText("Hello Text!", -0.98, 0.8f, 0.8);
  DrawEntities();
  renderer.DrawText();
  renderer.DrawXYZAxesOverlay();
}

void QuixotismEngine::DrawEntities() {
  QuixotismRenderer::GetRenderer().PrepareDrawStaticMeshes();
  for (auto& entity : entity_mgr) {
    auto* sm_comp = entity.GetComponent<StaticMeshComponent>();
    if (!sm_comp) continue;
    QuixotismRenderer::GetRenderer().DrawStaticMesh(sm_comp->GetStaticMeshId(),
                                                    entity.transform);
  }
}

void QuixotismEngine::InitTextFonts() {
  auto ttf_file = services.read_file("C:/Windows/Fonts/cour.ttf");
  if (ttf_file.data) {
    if (auto _font = TTFMakeASCIIFont(ttf_file.data.get(), ttf_file.size);
        _font.has_value()) {
      font = std::move(*_font);
    } else {
      DBG_PRINT("fialed to load font...");
    }
  }
}

void QuixotismEngine::DrawText(std::string&& text, r32 x, r32 y, r32 scale) {
  QuixotismRenderer::GetRenderer().PushText(std::move(text), Vec2{x, y}, scale);
}

}  // namespace quixotism
