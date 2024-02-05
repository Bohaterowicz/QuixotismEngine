#pragma once

#include <string>

#include "core/camera.hpp"
#include "core/input.hpp"
#include "core/platform_services.hpp"
#include "fonts/font.hpp"

namespace quixotism {

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

  Camera& GetCamera() { return camera; }

  void DrawText(std::string&& text, float x, float y, r32 scale);

  Font font;

  PlatformServices services;

 private:
  QuixotismEngine() {}

  Camera camera;

  void InitTextFonts();

  WindowDim window_dim{};
};

}  // namespace quixotism
