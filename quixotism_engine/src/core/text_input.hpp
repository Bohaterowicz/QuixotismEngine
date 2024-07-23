#pragma once

#include <functional>
#include <string>

#include "core/gui_interactive.hpp"

namespace quixotism {

class TextInput : public GUI_Interactive {
 public:
  TextInput() = default;

  void Init(std::function<void(std::string)> func) { commit_func = func; }

  void ProcessInput(InputState &input) override;
  void Update(r32 offset, r32 scale) override;

 private:
  std::string buffer;
  std::function<void(std::string)> commit_func;
};

}  // namespace quixotism
