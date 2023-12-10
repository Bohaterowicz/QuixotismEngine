#pragma once

#include "core/platform_services.hpp"
#include "fonts/font.hpp"

namespace quixotism {

struct WindowDim {
  i32 width;
  i32 height;
};

class QuixotismEngine {
 public:
  static QuixotismEngine& GetEngine() {
    static QuixotismEngine engine{};
    return engine;
  }

  void Init(PlatformServices& s);

  QuixotismEngine(const QuixotismEngine&) = delete;
  QuixotismEngine& operator=(const QuixotismEngine&) = delete;

  void UpdateAndRender();

  void UpdateWindowSize(i32 width, i32 height) { window_dim = {width, height}; }
  WindowDim GetWindowDim() const { return window_dim; }

  Font font;

 private:
  QuixotismEngine() {}

  void InitTextFonts();

  PlatformServices services;
  WindowDim window_dim{};
};

}  // namespace quixotism
