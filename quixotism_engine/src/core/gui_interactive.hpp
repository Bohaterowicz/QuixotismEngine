#pragma once

#include "core/input.hpp"

namespace quixotism {

class GUI_Interactive {
 public:
  GUI_Interactive() = default;

  void SetFocus(bool focus);

  virtual void ProcessInput(ControllerInput &input){};
  virtual void Update(r32 offset, r32 scale) = 0;

  virtual ~GUI_Interactive() = default;

 private:
};

}  // namespace quixotism
