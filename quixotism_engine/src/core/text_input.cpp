#include "text_input.hpp"

#include "core/quixotism_engine.hpp"

namespace quixotism {

void TextInput::Update(r32 offset, r32 scale) {
  if (buffer.size() > 0) {
    QuixotismEngine::GetEngine().DrawText(buffer, -0.99, offset, scale, 1);
  }
}

void TextInput::ProcessInput(ControllerInput &input) {
  if (input.enter.half_transition_count > 0 && input.enter.ended_down) {
    input.enter.half_transition_count = 0;
    commit_func(buffer);
    buffer.clear();
  } else {
    if (input.transition && input.ascii_code != -1) {
      input.transition = false;
      buffer.append(1, (char)input.ascii_code);
    }
  }
}

}  // namespace quixotism
