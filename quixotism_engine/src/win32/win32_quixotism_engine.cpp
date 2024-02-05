#include <Windows.h>

#include <format>
#include <memory>

#include "core/input.hpp"
#include "core/platform_services.hpp"
#include "core/quixotism_engine.hpp"
#include "dbg_print.hpp"
#include "quixotism_c.hpp"
#include "win32/win32_quixotism_io.hpp"
#include "win32/win32_quixotism_time.hpp"
#include "win32/win32_quixotism_window.hpp"

namespace quixotism::win32 {

static bool OpenDebugConsole() {
  bool result = AllocConsole();
  if (result) {
    auto *stream = stdout;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    DBG_PRINT("-----> Quixotism DEBUG console! <-----");
  }
  return result;
}

static PlatformServices InitPlatformServices() {
  PlatformServices platform_services{};
  platform_services.read_file = Win32ReadFile;
  platform_services.write_file = Win32WriteFile;
  platform_services.get_file_metadata = Win32GetFileMetadata;
  platform_services.get_world_timestamp = Win32GetWorldTimestamp;
  return platform_services;
}

}  // namespace quixotism::win32

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
#ifndef NDEBUG
  const auto debug_console_ok = quixotism::win32::OpenDebugConsole();
  if (debug_console_ok) {
    DBG_PRINT("DEBUG BUILD");
  } else {
    return 1;
  }
#endif  // NDEBUG

  // Construct Window
  quixotism::win32::Win32QuixotismWindow quixotism_window{hInstance};
  if (!quixotism::win32::Win32RegisterAndCreateWindow(quixotism_window)) {
    // Failed to regesiter or create windows window -> return with error
    return 1;
  }

  auto platform_services = quixotism::win32::InitPlatformServices();

  const auto [width, height] = quixotism_window.GetDimensions();
  auto &engine = quixotism::QuixotismEngine::GetEngine();
  quixotism::WindowDim window_dim{.width = width, .height = height};
  engine.Init(platform_services, window_dim);

  quixotism::ControllerInput input[2] = {};
  quixotism::ControllerInput *new_input = &input[0];
  quixotism::ControllerInput *prev_input = &input[1];

  POINT center_pos = { width / 2, height / 2 };
  ClientToScreen(quixotism_window.GetHandle(), &center_pos);
  SetCursorPos(center_pos.x, center_pos.y);

  i64 perf_freq = _Query_perf_frequency();
  i64 start_counter = _Query_perf_counter();
  r32 delta_t = 0;

  // Main engine loop, we keep running as long as the window was not requested
  // to close or the engine itself shutdown
  while (!quixotism_window.ShutdownRequested()) {
    quixotism_window.ProcessWindowMessages(*new_input);
    quixotism_window.ProcessMouseMovement(*new_input);
    engine.UpdateAndRender(*new_input, delta_t);
    HDC window_dc = GetDC(quixotism_window.GetHandle());
    SwapBuffers(window_dc);
    ReleaseDC(quixotism_window.GetHandle(), window_dc);
    SwapPointers(&new_input, &prev_input);

    i64 end_counter = _Query_perf_counter();
    i64 counter_elapsed = end_counter - start_counter;
    delta_t = static_cast<r32>(counter_elapsed) / static_cast<r32>(perf_freq);

    auto buffer = std::format("md_delta: {}\n", delta_t);
    OutputDebugStringA(buffer.c_str());
    start_counter = end_counter;
  }

  return 0;
}
