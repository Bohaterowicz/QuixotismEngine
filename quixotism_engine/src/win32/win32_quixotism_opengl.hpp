#pragma once
#include <Windows.h>

#include "quixotism_c.hpp"

namespace quixotism::win32 {

// Opengl init parameters

// OpenGL major version
static constexpr i32 OPENGL_DESIRED_MAJOR_VERION = 4;
// OpenGL minor version
static constexpr i32 OPENGL_DESIRED_MINOR_VERION = 5;

[[nodiscard]] auto Win32InitializeOpenGL(HWND WindowHandle) -> bool;

}  // namespace quixotism::win32