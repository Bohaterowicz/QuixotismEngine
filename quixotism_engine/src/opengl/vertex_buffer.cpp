#include "vertex_buffer.hpp"
#include "opengl_util.hpp"
#include <GL/glew.h>

vertex_buffer::vertex_buffer() : gl_buffer{GL_ARRAY_BUFFER}
{
}

vertex_buffer::vertex_buffer(vertex_buffer &&Other) : gl_buffer{std::forward<gl_buffer>(Other)}
{
}

vertex_buffer &vertex_buffer::operator=(vertex_buffer &&Other)
{
    if (this != &Other)
    {
        Release();
        std::swap(GetGLID(), Other.GetGLID());
        BufferSize = Other.BufferSize;
    }
    return *this;
}