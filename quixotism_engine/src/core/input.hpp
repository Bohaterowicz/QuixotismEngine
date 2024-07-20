#pragma once

#include "quixotism_c.hpp"

namespace quixotism {

static constexpr u32 SUPPORTED_CONTROLLERS_BUTTONS_COUNT = 10;

struct ButtonState {
  int half_transition_count;
  bool ended_down;
};

struct ControllerInput {
  union {
    ButtonState Buttons[SUPPORTED_CONTROLLERS_BUTTONS_COUNT];
    struct {
      ButtonState up;
      ButtonState down;
      ButtonState left;
      ButtonState forward;
      ButtonState backward;
      ButtonState right;
      ButtonState a;
      ButtonState b;
      ButtonState x;
      ButtonState y;
      ButtonState bb;

      // IMPORTANT: All buttons must be added before this line!
      ButtonState _END;
    };
  };

  r32 mouse_x_delta;
  r32 mouse_y_delta;
};

}  // namespace quixotism
