#include "win32_quixotism_window.hpp"

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

void Win32QuixotismWindow::ProcessWindowMessages() {
  MSG message;
  while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE) == TRUE) {
    switch (message.message) {
      default: {
        TranslateMessage(&message);
        DispatchMessageA(&message);
      } break;
    }
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