#include "quixotism_engine.hpp"

#include "core/constants.hpp"
#include "core/gui_interactive.hpp"
#include "core/texture.hpp"
#include "dbg_print.hpp"
#include "enumerate.hpp"
#include "file_processing/obj_parser/obj_parser.hpp"
#include "file_processing/png_parser/png_parser.hpp"
#include "renderer/quixotism_renderer.hpp"

namespace quixotism {

struct OBB {
  Vec3 center = {};
  Vec3 extents = {};
  // Orthonormal basis
  Vec3 axes[3] = {};
};

// SAT = Seperating Axes Theorem
static bool SATFrustumCulling(const FrustumDesc& frustum, const Mat4& transform,
                              const BoundingBox& bb) {
  Vec3 corners[] = {{bb.min.x, bb.min.y, bb.min.z},
                    {bb.max.x, bb.min.y, bb.min.z},
                    {bb.min.x, bb.max.y, bb.min.z},
                    {bb.min.x, bb.min.y, bb.max.z}};

  UnrollRaw<0, ArrayCount(corners)>(
      [&]<size_t i>() { corners[i] = (transform * Vec4(corners[i], 1)).xyz; });

  OBB obb = {
      .axes = {corners[1] - corners[0], corners[2] - corners[0],
               corners[3] - corners[0]},
  };

  obb.center = corners[0] + 0.5f * (obb.axes[0] + obb.axes[1] + obb.axes[2]);
  obb.extents =
      Vec3{obb.axes[0].Length(), obb.axes[1].Length(), obb.axes[2].Length()};
  obb.axes[0] = obb.axes[0] / obb.extents.x;
  obb.axes[1] = obb.axes[1] / obb.extents.y;
  obb.axes[2] = obb.axes[2] / obb.extents.z;
  obb.extents *= 0.5f;

  // First test near and far planes of the frustum (along z-axis) (easy)
  {
    // When testing near and far planes, we are projecting bb of object onto
    // z-axis, thus we get the ojects bb center z-value as its projected center
    auto projected_center = obb.center.z;
    r32 radius = 0;
    UnrollRaw<0, 3>(
        [&]<size_t i>() { radius += Abs(obb.axes[i].z) * obb.extents[i]; });

    r32 extent_far = projected_center - radius;
    r32 extent_near = projected_center + radius;

    if (extent_far > (-frustum.near_plane) || extent_near < (-frustum.far_plane))
      return false;
  }
  return true;
}

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
  camera_id2 = entity_mgr.Clone(camera_id);

  InitTextFonts();

  auto png_data = QuixotismEngine::GetEngine().services.read_file(
      "D:/QuixotismEngine/quixotism_engine/data/textures/wood_box.png");
  auto img = ParsePNG(png_data.data.get(), png_data.size);
  if (img) {
    Texture texture{std::move(img.value())};
    texture.glid = CreateTexture(std::get<Bitmap>(texture.bitmap), false);
    tex_id = texture_mgr.Add(std::move(texture));
  }

  CubeBitmap cube_bitmaps;
  for (auto [i, bitmap] : Enumerate(cube_bitmaps)) {
    auto png_data = QuixotismEngine::GetEngine().services.read_file(
        ("D:/QuixotismEngine/quixotism_engine/data/textures/yokohama/" +
         std::to_string(i) + ".png")
            .c_str());
    auto img = ParsePNG(png_data.data.get(), png_data.size);
    if (img) {
      bitmap = std::move(img.value());
    } else {
      Assert(!"could not load image");
    }
  }
  Texture texture{std::move(cube_bitmaps)};
  texture.glid = CreateCubeTexture(std::get<CubeBitmap>(texture.bitmap));
  ctex_id = texture_mgr.Add(std::move(texture));

  auto obj_data = QuixotismEngine::GetEngine().services.read_file(
      "D:/QuixotismEngine/quixotism_engine/data/meshes/box.obj");
  auto meshes = ParseOBJ(obj_data.data.get(), obj_data.size);

  StaticMesh smesh{std::move(meshes[0])};
  auto mesh_id = static_mesh_mgr.Add(std::move(smesh));
  QuixotismRenderer::GetRenderer().MakeDrawableStaticMesh(mesh_id);

  obj_data = QuixotismEngine::GetEngine().services.read_file(
      "D:/QuixotismEngine/quixotism_engine/data/meshes/sphere64.obj");
  meshes = ParseOBJ(obj_data.data.get(), obj_data.size);

  smesh = StaticMesh{std::move(meshes[0])};
  auto s_mesh_id = static_mesh_mgr.Add(std::move(smesh));
  QuixotismRenderer::GetRenderer().MakeDrawableStaticMesh(s_mesh_id);

  Material mat1{QuixotismRenderer::GetRenderer().shader_mgr.GetByName("model")};
  mat1.diffuse = tex_id;
  auto mat1_id = material_mgr.Add(std::move(mat1));

  Entity box;
  StaticMeshComponent box_sm{mesh_id, mat1_id};
  box.AddComponent(box_sm);
  box_id = entity_mgr.Add(std::move(box));
  auto box_id2 = entity_mgr.Clone(box_id);
  entity_mgr.Get(box_id2)->transform.Move(Vec3{100, 100, 100});

  Entity sphere;
  StaticMeshComponent sphere_sm{s_mesh_id, mat1_id};
  sphere.AddComponent(sphere_sm);
  entity_mgr.Add(std::move(sphere));

  terminal.Init();
}

void QuixotismEngine::UpdateAndRender(InputState& input, r32 delta_t) {
  auto& renderer = QuixotismRenderer::GetRenderer();
  rendered_entities_count = 0;
  auto& transform = entity_mgr.Get(camera_id)->transform;

  auto speed = 50.0F;  // m/s
  auto rotation_speed = 1.0F;
  auto movement = Vec3{0.0F};

  if (input.key_state_info['T'].is_down &&
      input.key_state_info['T'].transition &&
      input.key_state_info[KC_CONTROL].is_down) {
    DBG_PRINT("Toggle terminal...");
    terminal.ToggleShow();
  }

  if (!focused_element) {
    if (input.key_state_info['Q'].is_down) {
      movement += UP;
    }
    if (input.key_state_info['E'].is_down) {
      movement += -UP;
    }
    if (input.key_state_info['W'].is_down) {
      movement += transform.Forward();
    }
    if (input.key_state_info['S'].is_down) {
      movement += -transform.Forward();
    }
    if (input.key_state_info['D'].is_down) {
      movement += transform.Right();
    }
    if (input.key_state_info['A'].is_down) {
      movement += -transform.Right();
    }
  } else {
    focused_element->ProcessInput(input);
  }

  if (movement.Length() > 0.0F) {
    transform.position += (movement.Normalize() * speed * delta_t);
  }

  auto rotation_delta = Vec3{-input.mouse_y_delta, -input.mouse_x_delta, 0} *
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

  if (input.key_state_info['B'].is_down &&
      input.key_state_info['B'].transition) {
    show_bb = !show_bb;
  }

  terminal.Update(delta_t);

  renderer.ClearRenderTarget();
  DrawText("Hello Text!", -0.98, 0.8f, 0.03448);
  DrawEntities();
  DrawText("entity draw count: " + std::to_string(rendered_entities_count),
           -0.98, 0.7f, 0.009);
  renderer.DrawSkybox();
  renderer.DrawText(0);
  renderer.DrawXYZAxesOverlay();

  if (terminal.IsVisible()) {
    renderer.DrawTerminal(terminal.smid, terminal.matid,
                          terminal.DrawTransform());
    renderer.DrawText(1);
  }

  renderer.ClearTextBuffer();
}

void QuixotismEngine::DrawEntities() {
  auto camera_id = GetCamera();
  auto* camera = entity_mgr.GetComponent<CameraComponent>(camera_id);
  auto& transform = entity_mgr.Get(camera_id)->transform;
  auto view = transform.GetTransformMatrix();
  QuixotismRenderer::GetRenderer().PrepareDrawStaticMeshes();
  for (auto& entity : entity_mgr) {
    auto* sm_comp = entity.GetComponent<StaticMeshComponent>();
    if (!sm_comp) continue;

    auto* sm = static_mesh_mgr.Get(sm_comp->GetStaticMeshId());
    auto transform = view * entity.transform.GetTransformMatrix();
    if (SATFrustumCulling(camera->GetFrustumDescription(), transform,
                          sm->GetMeshData().bbox)) {
      QuixotismRenderer::GetRenderer().DrawStaticMesh(
          sm_comp->GetStaticMeshId(), sm_comp->GetMaterialID(),
          entity.transform);
      ++rendered_entities_count;
    }
  }
  if (show_bb) {
    for (auto& entity : entity_mgr) {
      auto* sm_comp = entity.GetComponent<StaticMeshComponent>();
      if (!sm_comp) continue;
      QuixotismRenderer::GetRenderer().DrawBoundingBox(
          sm_comp->GetStaticMeshId(), entity.transform);
    }
  }
  /*
  auto* camera2 = entity_mgr.GetComponent<CameraComponent>(camera_id2);
  auto transform =
      view * entity_mgr.Get(camera_id)->transform.GetTransformMatrix();
  QuixotismRenderer::GetRenderer().DrawCameraFrustum(
      camera2->GetFrustumDescription(), transform);
      */
}

void QuixotismEngine::InitTextFonts() {
  auto ttf_file = services.read_file("C:/Windows/Fonts/cour.ttf");
  if (ttf_file.data) {
    if (auto _font = TTFMakeASCIIFont(ttf_file.data.get(), ttf_file.size);
        _font.has_value()) {
      font_mgr.Add(std::move(*_font));
    } else {
      DBG_PRINT("fialed to load font...");
    }
  }
}

void QuixotismEngine::DrawText(std::string text, r32 x, r32 y, r32 scale,
                               u32 layer, u64 font_id) {
  QuixotismRenderer::GetRenderer().PushText(std::move(text), Vec2{x, y}, scale,
                                            layer, font_id);
}

void QuixotismEngine::DrawText(std::string text, r32 x, r32 y, Vec3 color,
                               r32 scale, u32 layer, u64 font_id) {
  QuixotismRenderer::GetRenderer().PushText(std::move(text), Vec2{x, y}, color,
                                            scale, layer, font_id);
}

}  // namespace quixotism
