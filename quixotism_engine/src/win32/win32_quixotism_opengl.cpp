#include "win32_quixotism_opengl.hpp"

#include <array>
#include <string>

#include "GL/glew.h"
#include "GL/wglew.h"
#include "dbg_print.hpp"

namespace quixotism::win32 {

auto Win32InitializeOpenGL(HWND window_handle) -> bool {
  bool result = false;

  HDC window_dc = GetDC(window_handle);
  // Creat opengl context
  HGLRC opengl_rendering_context = wglCreateContext(window_dc);

  // Check if we got valid rendering context
  if (opengl_rendering_context) {
    if (wglMakeCurrent(window_dc, opengl_rendering_context) != 0) {
      // initialize glew, if we fail, we should stop initialization of opengl,
      // as we need glew right now.
      if (glewInit() == GLEW_OK) {
        const u32 WGL_CONTEXT_ATTRIBS_COUNT = 9;
        // Context attribs, we specify the version we want to use...
        std::array<i32, WGL_CONTEXT_ATTRIBS_COUNT> wgl_context_attribs = {
            WGL_CONTEXT_MAJOR_VERSION_ARB,
            OPENGL_DESIRED_MAJOR_VERION,
            WGL_CONTEXT_MINOR_VERSION_ARB,
            OPENGL_DESIRED_MINOR_VERION,
            WGL_CONTEXT_FLAGS_ARB,
            0,
            WGL_CONTEXT_PROFILE_MASK_ARB,
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0};

        // Check for "WGL_ARB_create_context" as we may want to specify our
        // own version of opengl
        if (wglewIsSupported("WGL_ARB_create_context") == GL_TRUE) {
          // Create context with specified attributes
          HGLRC opengl_rendering_context_arb = wglCreateContextAttribsARB(
              window_dc, nullptr, wgl_context_attribs.data());
          if (opengl_rendering_context_arb) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(opengl_rendering_context);
            wglMakeCurrent(window_dc, opengl_rendering_context_arb);
          }
        } else {
          DBG_PRINT(
              "----------> wglCreateContextAtribs not specified (WARNING)");
        }
        // We scucceded in initializing opengl in our window
        // Print out opengl version...
        const auto *gl_version = glGetString(GL_VERSION);
        std::string ogl_text("OpenGL VERSION: ");
        std::string gl_version_str(reinterpret_cast<const char *>(gl_version));
        DBG_PRINT((ogl_text + gl_version_str + "\n").c_str());

        // NOTE: IMPORTANT: Activates vsync (swap buffers on vertical blank)!
        wglSwapIntervalEXT(1);

        result = TRUE;
      } else {
        // deinitialize the current rendering context
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(opengl_rendering_context);
        DBG_PRINT("GLEW could not be initialized... (ABORTING)");
      }
    }
  }
  ReleaseDC(window_handle, window_dc);

  return result;
}

}  // namespace quixotism::win32
