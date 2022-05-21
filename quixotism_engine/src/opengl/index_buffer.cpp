#include "index_buffer.hpp"
#include <GL/glew.h>

index_buffer::index_buffer() : gl_buffer{GL_ELEMENT_ARRAY_BUFFER}
{
}

index_buffer::index_buffer(index_buffer &&Other) : gl_buffer{std::forward<gl_buffer>(Other)}
{
}

index_buffer &index_buffer::operator=(index_buffer &&Other)
{
    if (this != &Other)
    {
        Release();
        std::swap(GetGLID(), Other.GetGLID());
        BufferSize = Other.BufferSize;
    }
    return *this;
}