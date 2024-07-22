#pragma once

#include <string>

#include "core/entity_manager.hpp"
#include "core/input.hpp"
#include "core/material_manager.hpp"
#include "core/platform_services.hpp"
#include "core/static_mesh_manager.hpp"
#include "core/terminal.hpp"
#include "core/texture_manager.hpp"
#include "fonts/font.hpp"

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

  void UpdateAndRender(ControllerInput& input, r32 delta_t);

  void UpdateWindowSize(i32 width, i32 height) { window_dim = {width, height}; }
  WindowDim GetWindowDim() const { return window_dim; }

  EntityId GetCamera() const { return camera_id; }

  void DrawText(std::string text, float x, float y, r32 scale, u32 layer = 0);

  void DrawEntities();

  void SetElementFocus(GUI_Interactive* element) { focused_element = element; }

  Font font;

  PlatformServices services;
  EntityManager entity_mgr;
  StaticMeshManager static_mesh_mgr;
  MaterialManager material_mgr;
  TextureManager texture_mgr;

  u64 tex_id;

  bool show_bb = false;
  u32 show_terminal = 0;
  Terminal terminal;

 private:
  QuixotismEngine() {}

  GUI_Interactive* focused_element;

  EntityId camera_id, box_id;

  void InitTextFonts();

  WindowDim window_dim{};
};

}  // namespace quixotism
