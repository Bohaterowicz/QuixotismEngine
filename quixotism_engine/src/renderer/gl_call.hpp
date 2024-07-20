#pragma once
#include <GL/glew.h>

#include <cAssert>
#include <print>

#include "quixotism_c.hpp"

#ifndef NDEBUG
#define GLCall(x) \
  GLClearError(); \
  x;              \
  Assert(GLLogCall(#x, __FILE__, __LINE__));
#else
#define GLCall(x) x;
#endif

inline void GLClearError() {
  while (glGetError() != GL_NO_ERROR) {
  }
}

inline bool GLLogCall(const char *function, const char *file, int line) {
  bool err = true;
  while (GLenum error = glGetError()) {
    std::print("OpenGL Error ({}): {} -> {} -> {}\n", error, file, line,
               function);
    err = FALSE;
  }
  return err;
}