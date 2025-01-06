#include "win32_quixotism_window.hpp"

#include <Windowsx.h>

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

void Win32QuixotismWindow::ProcessWindowMessages(InputState &input) {
  for (auto &key : input.key_state_info) {
    key.transition = false;
  }
  MSG message;
  while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE) == TRUE) {
    switch (message.message) {
      case WM_LBUTTONDOWN: {
        auto &key = input.key_state_info[KC_LBUTTON];
        key.mouse_pos.x = GET_X_LPARAM(message.lParam);
        key.mouse_pos.y = GET_Y_LPARAM(message.lParam);
        key.transition = key.is_down != (message.wParam & 0x0001);
        key.is_down = message.wParam & 0x0001;
      } break;
      case WM_LBUTTONUP: {
        auto &key = input.key_state_info[KC_LBUTTON];
        key.transition = key.is_down;
        key.is_down = false;
      } break;
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
        bool alt_key_was_down =
            ((message.lParam & (1 << ALT_KEY_IS_DOWN_SHIFT)) != 0);

        if ((vk_code == VK_ESCAPE) || (vk_code == VK_F4 && alt_key_was_down)) {
          Shutdown();
        }
        auto &key = input.key_state_info[vk_code];
        // 30th bit in LParam indicates if the key was pressed down before
        // (previous state)
        bool was_down = ((message.lParam & (1 << WAS_DOWN_BIT_SHIFT)) != 0);
        // 31st bit in LParam indicates if the key is currently down (current
        // state)
        bool is_down = ((message.lParam & (1 << IS_DOWN_BIT_SHIFT)) == 0);

        key.is_down = is_down;
        key.transition = was_down != is_down;
        input.last_key_code = (KeyCode)vk_code;
      } break;
      default: {
        TranslateMessage(&message);
        DispatchMessageA(&message);
      } break;
    }
  }
}

void Win32QuixotismWindow::ProcessMouseMovement(InputState &input) {
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