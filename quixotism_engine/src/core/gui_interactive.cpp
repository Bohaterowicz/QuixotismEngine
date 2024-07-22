#include "gui_interactive.hpp"

#include "core/quixotism_engine.hpp"

namespace quixotism {

void GUI_Interactive::SetFocus(bool focus) {
  if (focus) {
    QuixotismEngine::GetEngine().SetElementFocus(this);
  } else {
    QuixotismEngine::GetEngine().SetElementFocus(nullptr);
  }
}

}  // namespace quixotism
