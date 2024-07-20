#include "win32_quixotism_window.hpp"

#include "dbg_print.hpp"
#include "win32_quixotism_opengl.hpp"

namespace quixotism::win32 {

static bool Win32SetPixelFormat(HWND window) {
  PIXELFORMATDESCRIPTOR pixel_format_desc = {};
  pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pixel_format_desc.nVersion = 1;
  pixel_format_desc.dwFlags =
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
  pixel_format_desc.cColorBits = pixelformat::COLOR_BITS;
  pixel_format_desc.cAlphaBits = pixelformat::ALPHA_BITS;
  pixel_format_desc.cDepthBits = pixelformat::DEPTH_BITS;
  pixel_format_desc.cStencilBits = pixelformat::STENCIL_BITS;
  pixel_format_desc.iLayerType = PFD_MAIN_PLANE;
  HDC window_dc = GetDC(window);
  i32 pixel_format_idx = ChoosePixelFormat(window_dc, &pixel_format_desc);
  DescribePixelFormat(window_dc, pixel_format_idx, sizeof(pixel_format_desc),
                      &pixel_format_desc);
  // Check if we got a valid suggestion of pixel format
  if (pixel_format_idx != 0) {
    // continue only if we succeed in setting the pixel format
    if (SetPixelFormat(window_dc, pixel_format_idx, &pixel_format_desc)) {
      ReleaseDC(window, window_dc);
      return true;
    }
  }
  ReleaseDC(window, window_dc);
  return false;
}

static void Win32ProcessKeyboardInput(ButtonState &new_state, bool is_down,
                                      bool transition = true) {
  new_state.half_transition_count = transition ? 1 : 0;
  if (new_state.ended_down != is_down) {
    new_state.ended_down = is_down;
  }
}

void Win32QuixotismWindow::ProcessWindowMessages(ControllerInput &input) {
  MSG message;
  while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE) == TRUE) {
    switch (message.message) {
      case WM_RBUTTONDOWN: {
        camera_control_mode = true;
        i32 center_x = width / 2;
        i32 center_y = height / 2;
        POINT center_pos = {center_x, center_y};
        ClientToScreen(handle, &center_pos);
        SetCursorPos(center_pos.x, center_pos.y);
      } break;
      case WM_RBUTTONUP: {
        camera_control_mode = false;
      } break;
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      case WM_KEYDOWN:
      case WM_KEYUP: {
        constexpr u32 WAS_DOWN_BIT_SHIFT = 30;
        constexpr u32 IS_DOWN_BIT_SHIFT = 31;
        constexpr u32 ALT_KEY_IS_DOWN_SHIFT = 29;
        auto vk_code = static_cast<u32>(message.wParam);
        // 30th bit in LParam indicates if the key was pressed down before
        // (previous state)
        bool was_down = ((message.lParam & (1 << WAS_DOWN_BIT_SHIFT)) != 0);
        // 31st bit in LParam indicates if the key is currently down (current
        // state)
        bool is_down = ((message.lParam & (1 << IS_DOWN_BIT_SHIFT)) == 0);

        bool alt_key_was_down =
            ((message.lParam & (1 << ALT_KEY_IS_DOWN_SHIFT)) != 0);

        if (vk_code == VK_ESCAPE) {
          Shutdown();
        }
        if (vk_code == VK_F4 && alt_key_was_down) {
          Shutdown();
        }

        // If the key was down before, but now isnt, that means that the key got
        // released, and the other way around This if statment pervents us from
        // being flooded by key-repeat messages (meaning that a key is being
        // held down)
        bool transition = was_down != is_down;
        if (transition) {
          if (vk_code == 'C') {
            if (is_down) {
              DBG_PRINT("TOGGLING MOUSE VISIBILITY");
              /*
              Win32ProcessKeyboardInput(KeyboardController.Start, is_down);
              AppState.ToggleCursorHidden();
              if (AppState.IsCursorHidden()) {
                ShowCursor(FALSE);
              } else {
                ShowCursor(TRUE);
              }
              */
            }
          }
          if (vk_code == 'E') {
            Win32ProcessKeyboardInput(input.up, is_down);
          }
          if (vk_code == 'Q') {
            Win32ProcessKeyboardInput(input.down, is_down);
          }
          if (vk_code == 'W') {
            Win32ProcessKeyboardInput(input.forward, is_down);
          }
          if (vk_code == 'S') {
            Win32ProcessKeyboardInput(input.backward, is_down);
          }
          if (vk_code == 'D') {
            Win32ProcessKeyboardInput(input.right, is_down);
          }
          if (vk_code == 'A') {
            Win32ProcessKeyboardInput(input.left, is_down);
          }
        }
        if (vk_code == 'B') {
          Win32ProcessKeyboardInput(input.bb, is_down, transition);
        }
        if (vk_code == 'T') {
          Win32ProcessKeyboardInput(input.tt, is_down, transition);
        }
      } break;
      default: {
        TranslateMessage(&message);
        DispatchMessageA(&message);
      } break;
    }
  }
}

void Win32QuixotismWindow::ProcessMouseMovement(ControllerInput &input) {
  if (camera_control_mode) {
    POINT mouse_pos = {};
    GetCursorPos(&mouse_pos);
    ScreenToClient(handle, &mouse_pos);
    i32 center_x = width / 2;
    i32 center_y = height / 2;

    input.mouse_x_delta = mouse_pos.x - center_x;
    input.mouse_y_delta = mouse_pos.y - center_y;

    POINT center_pos = {center_x, center_y};
    ClientToScreen(handle, &center_pos);
    SetCursorPos(center_pos.x, center_pos.y);
  } else {
    input.mouse_x_delta = 0;
    input.mouse_y_delta = 0;
  }
}

// Main window callback procedure
LRESULT CALLBACK Win32QuixotismEngineWindowProc(HWND window_handle,
                                                UINT message, WPARAM wparam,
                                                LPARAM lparam) {
  LRESULT result = {};

  switch (message) {
    case WM_CREATE: {
      auto *create = reinterpret_cast<CREATESTRUCT *>(lparam);
      auto *window =
          reinterpret_cast<Win32QuixotismWindow *>(create->lpCreateParams);
      SetWindowLongPtr(window_handle, GWLP_USERDATA,
                       reinterpret_cast<LONG_PTR>(window));
      if (!Win32SetPixelFormat(window_handle) ||
          !Win32InitializeOpenGL(window_handle)) {
        // Could not set pixel format or initialize opengl.. abort
        PostQuitMessage(-1);
      }
    } break;
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP: {
      Assert(!"Keyboard event came from wrong dispatch......");
    } break;
    default: {
      result = DefWindowProcA(window_handle, message, wparam, lparam);
    } break;
  }

  return result;
}

bool Win32RegisterAndCreateWindow(Win32QuixotismWindow &window) {
  // Creating our window class
  WNDCLASSEXA window_class = {};
  window_class.cbSize = sizeof(WNDCLASSEXA);
  window_class.style = CS_OWNDC;
  window_class.lpfnWndProc = Win32QuixotismEngineWindowProc;
  window_class.hInstance = window.GetInstance();
  window_class.hCursor = LoadCursorA(nullptr, IDC_ARROW);  // NOLINT
  window_class.lpszClassName = "QuixotismEngineWindowClass";

  // Registering window class
  if (!RegisterClassExA(&window_class)) {
    // failed to register window -> return error
    return false;
  }

  const auto [width, height] = window.GetDimensions();
  RECT init_window_size;
  init_window_size.left = Win32QuixotismWindow::DEFAULT_WINDOW_POS_X;
  init_window_size.right = Win32QuixotismWindow::DEFAULT_WINDOW_POS_X + width;
  init_window_size.top = Win32QuixotismWindow::DEFAULT_WINDOW_POS_Y;
  init_window_size.bottom = Win32QuixotismWindow::DEFAULT_WINDOW_POS_Y + height;
  AdjustWindowRectEx(&init_window_size, WS_OVERLAPPEDWINDOW | WS_VISIBLE, false,
                     0);

  // Create window
  HWND window_handle = CreateWindowExA(
      0, window_class.lpszClassName, "QuixotismEngine",
      (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS) &
          ~WS_THICKFRAME,
      init_window_size.left, init_window_size.top,
      (init_window_size.right - init_window_size.left),
      (init_window_size.bottom - init_window_size.top), nullptr, nullptr,
      window.GetInstance(), &window);

  if (!window_handle) {
    // failed to create window
    return false;
  }

  window.SetHandle(window_handle);
  return true;
}

}  // namespace quixotism::win32