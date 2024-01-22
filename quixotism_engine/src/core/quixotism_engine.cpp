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
  DrawText("Hyello Text! Much to do porter, ying yang. jhon", -0.98, 0.8f,
           0.5f);
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

void QuixotismEngine::DrawText(std::string&& text, r32 x, r32 y, r32 scale) {
  QuixotismRenderer::GetRenderer().PushText(std::move(text), Vec2{x, y}, scale);
}

}  // namespace quixotism
