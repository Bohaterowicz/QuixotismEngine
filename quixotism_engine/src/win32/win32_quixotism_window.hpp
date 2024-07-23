#pragma once
#include <Windows.h>

#include <utility>

#include "core/input.hpp"

namespace quixotism::win32 {

namespace pixelformat {

static constexpr u32 COLOR_BITS = 32;
static constexpr u32 ALPHA_BITS = 8;
static constexpr u32 DEPTH_BITS = 24;
static constexpr u32 STENCIL_BITS = 8;

}  // namespace pixelformat

class Win32QuixotismWindow {
 public:
  explicit Win32QuixotismWindow(HINSTANCE instance) : instance{instance} {}

  explicit Win32QuixotismWindow(HINSTANCE instance, i32 width, i32 height)
      : instance{instance}, width{width}, height{height} {}

  [[nodiscard]] auto GetInstance() const { return instance; }
  [[nodiscard]] auto GetHandle() const { return handle; }
  void SetHandle(HWND window_handle) { handle = window_handle; }
  [[nodiscard]] auto GetDimensions() const -> std::pair<i32, i32> {
    return std::make_pair(width, height);
  }

  void Shutdown() { shutdown_requested = true; }
  [[nodiscard]] bool ShutdownRequested() const { return shutdown_requested; }

  void ProcessWindowMessages(InputState &input);
  void ProcessMouseMovement(InputState &input);

  static constexpr i32 DEFAULT_WINDOW_WIDHT = 800;
  static constexpr i32 DEFAULT_WINDOW_HEIGHT = 600;
  static constexpr i32 DEFAULT_WINDOW_POS_X = 600;
  static constexpr i32 DEFAULT_WINDOW_POS_Y = 200;

 private:
  HINSTANCE instance;
  HWND handle;
  i32 width = DEFAULT_WINDOW_WIDHT;
  i32 height = DEFAULT_WINDOW_HEIGHT;

  bool shutdown_requested = false;

  bool camera_control_mode = false;
};

[[nodiscard]] bool Win32RegisterAndCreateWindow(Win32QuixotismWindow &window);

}  // namespace quixotism::win32