#include "gl_buffer.hpp"
#include "opengl_util.hpp"
#include <GL/glew.h>

gl_buffer::gl_buffer(int32 Target) : BindTarget{Target}
{
    GLCall(glCreateBuffers(1, &GetGLID()));
}

gl_buffer::gl_buffer(gl_buffer &&Other) : BindTarget{Other.BindTarget}, BufferSize{Other.BufferSize}
{
    GetGLID() = Other.GetGLID();
    Other.GetGLID() = 0;
}

gl_buffer &gl_buffer::operator=(gl_buffer &&Other)
{
    if (this != &Other)
    {
        Release();
        std::swap(GetGLID(), Other.GetGLID());
        BindTarget = Other.BindTarget;
        BufferSize = Other.BufferSize;
    }
    return *this;
}

gl_buffer::~gl_buffer()
{
    GLCall(glDeleteBuffers(1, &GetGLID()));
    GetGLID() = 0;
}

void gl_buffer::Bind() const
{
    GLCall(glBindBuffer(BindTarget, GetGLID()));
}

void gl_buffer::Unbind() const
{
    GLCall(glBindBuffer(BindTarget, 0));
}

void gl_buffer::BufferData(void *Data, size Size, int32 Usage)
{
    if (Data && (Size > 0))
    {
        GLCall(glNamedBufferData(GetGLID(), Size, Data, Usage));
    }
    else
    {
        Assert(!"Trying to buffer data when no data is specified...");
    }
}

void gl_buffer::BufferSubData(void *Data, size Offset, size Size, int Usage)
{
    if (Data && (Size > 0))
    {
        if ((Offset + Size <= BufferSize) && (Offset + Size > 0))
        {
            GLCall(glNamedBufferData(GetGLID(), Size, Data, Usage));
        }
        else
        {
            Assert(!"Trying to sub buffer data that would go outside the range of the buffer");
        }
    }
    else
    {
        Assert(!"Trying to buffer data when no data is specified...");
    }
}

/**
 * @brief Reserves memory on gpu with the given size, but does not copy any data...
 *
 * @param Usage
 */
void gl_buffer::ReserveData(size Size, int32 Usage)
{
    GLCall(glNamedBufferData(GetGLID(), Size, nullptr, Usage));
}

void gl_buffer::Release()
{
    GLCall(glDeleteBuffers(1, &GetGLID()));
    GetGLID() = 0;
}