#pragma once
#include "quixotism_c.hpp"
#include <GL/glew.h>
#include <cstdio>

#ifdef BUILD_INTERNAL
#define GLCall(x)                                                                                                      \
    GLClearError();                                                                                                    \
    x;                                                                                                                 \
    Assert(GLLogCall(#x, __FILE__, __LINE__));
#else
#define GLCall(x) x;
#endif

#ifdef BUILD_INTERNAL
#define GLDEBUG(msg) printf_s("GL_DEBUG: %s\n", msg);
#else
#define GLDEBUG(msg)
#endif

inline void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
    {
    }
}

inline bool32 GLLogCall(const char *Function, const char *File, int Line)
{
    bool32 Err = TRUE;
    while (GLenum Error = glGetError())
    {
        printf_s("OpenGL Error (%i): %s -> %i -> %s\n", Error, File, Line, Function);
        Err = FALSE;
    }
    return Err;
}