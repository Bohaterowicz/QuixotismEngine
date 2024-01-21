#include "quixotism_engine.hpp"

#include "dbg_print.hpp"
#include "renderer/quixotism_renderer.hpp"

namespace quixotism {

void QuixotismEngine::Init(PlatformServices& s) {
  services = s;

  InitTextFonts();
}

void QuixotismEngine::UpdateAndRender() {
  auto& renderer = QuixotismRenderer::GetRenderer();

  renderer.ClearRenderTarget();
  DrawText("Hello Text!", -0.7, 0.8f);
  renderer.Test();
}

void QuixotismEngine::InitTextFonts() {
  auto ttf_file = services.read_file("C:/Windows/Fonts/arial.ttf");
  if (ttf_file.data) {
    if (auto _font = TTFMakeASCIIFont(ttf_file.data.get(), ttf_file.size);
        _font.has_value()) {
      font = std::move(*_font);
    } else {
      DBG_PRINT("fialed to load font...");
    }
  }
}

void QuixotismEngine::DrawText(std::string&& text, r32 x, r32 y) {
  QuixotismRenderer::GetRenderer().PushText(std::move(text), Vec2{x, y});
}

}  // namespace quixotism
