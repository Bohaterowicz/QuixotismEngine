#pragma once

#include <string>

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

  void Init(PlatformServices& s);

  void UpdateAndRender();

  void UpdateWindowSize(i32 width, i32 height) { window_dim = {width, height}; }
  WindowDim GetWindowDim() const { return window_dim; }

  void DrawText(std::string&& text, float x, float y);

  Font font;

 private:
  QuixotismEngine() {}

  void InitTextFonts();

  PlatformServices services;
  WindowDim window_dim{};
};

}  // namespace quixotism
