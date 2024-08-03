#include "text_input.hpp"

#include "core/quixotism_engine.hpp"

namespace quixotism {

void TextInput::Update(r32 offset, r32 scale) {
  if (buffer.size() > 0) {
    QuixotismEngine::GetEngine().DrawText(buffer, -0.99, offset, scale, 1);
  }
}

void TextInput::ProcessInput(InputState &input) {
  if (input.key_state_info[KC_RETURN].is_down &&
      input.key_state_info[KC_RETURN].transition) {
    commit_func(buffer);
    buffer.clear();
  } else {
    if (input.key_state_info[KC_CONTROL].is_down) {
      return;
    }

    char c;
    if (ToCharacter(input.last_key_code, input.key_state_info[KC_SHIFT].is_down,
                    c) &&
        input.key_state_info[input.last_key_code].transition &&
        input.key_state_info[input.last_key_code].is_down) {
      buffer.append(1, c);
    } else if (input.last_key_code == KC_SPACE &&
               input.key_state_info[input.last_key_code].transition &&
               input.key_state_info[input.last_key_code].is_down) {
      buffer.append(1, ' ');
    } else if (input.last_key_code == KC_BACK &&
               input.key_state_info[input.last_key_code].transition &&
               input.key_state_info[input.last_key_code].is_down) {
      if (!buffer.empty()) buffer.pop_back();
    }
  }
}

}  // namespace quixotism
