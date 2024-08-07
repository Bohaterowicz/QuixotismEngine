#pragma once

#include <string>

#include "core/entity_manager.hpp"
#include "core/font_manager.hpp"
#include "core/input.hpp"
#include "core/material_manager.hpp"
#include "core/platform_services.hpp"
#include "core/static_mesh_manager.hpp"
#include "core/terminal.hpp"
#include "core/texture_manager.hpp"

namespace quixotism {

class GUI_Interactive;

struct WindowDim {
  i32 width;
  i32 height;
};

class QuixotismEngine {
 public:
  CLASS_DELETE_COPY(QuixotismEngine);
  static QuixotismEngine& GetEngine() {
    static QuixotismEngine engine{};
    return engine;
  }

  void Init(const PlatformServices& init_services, const WindowDim& dim);

  void UpdateAndRender(InputState& input, r32 delta_t);

  void UpdateWindowSize(i32 width, i32 height) { window_dim = {width, height}; }
  WindowDim GetWindowDim() const { return window_dim; }

  EntityId GetCamera() const { return camera_id; }
  EntityId GetCamera2() const { return camera_id2; }

  void DrawText(std::string text, float x, float y, r32 scale, u32 layer = 0,
                u64 font_id = 0);
  void DrawText(std::string text, r32 x, r32 y, Vec3 color, r32 scale,
                u32 layer, u64 font_id = 0);

  void DrawEntities();

  void SetElementFocus(GUI_Interactive* element) { focused_element = element; }

  PlatformServices services;
  EntityManager entity_mgr;
  StaticMeshManager static_mesh_mgr;
  MaterialManager material_mgr;
  TextureManager texture_mgr;
  FontManager font_mgr;

  u64 tex_id, ctex_id;

  bool show_bb = false;
  u32 show_terminal = 0;
  Terminal terminal;

  size_t rendered_entities_count;

 private:
  QuixotismEngine() {}

  GUI_Interactive* focused_element;

  EntityId camera_id, camera_id2, box_id;

  void InitTextFonts();

  WindowDim window_dim{};
};

}  // namespace quixotism
