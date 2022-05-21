#include "vertex_array.hpp"
#include "opengl_util.hpp"

vertex_array::vertex_array(vertex_buffer_layout &VBLayout)
{
    GLCall(glCreateVertexArrays(1, &GetGLID()));

    uint32 BlockLocation = 0;
    uint32 AttributeLocation = 0;

    for (const auto &Element : VBLayout.GetLayout())
    {
        GLCall(glEnableVertexArrayAttrib(GetGLID(), AttributeLocation));
        GLCall(glVertexArrayAttribFormat(GetGLID(), AttributeLocation, Element.Count, Element.Type,
                                         static_cast<GLboolean>(Element.Normalize),
                                         static_cast<GLuint>(Element.Offset)));
        GLCall(glVertexArrayAttribBinding(GetGLID(), AttributeLocation, BlockLocation));
        ++AttributeLocation;
    }
}

vertex_array::vertex_array(vertex_array &&Other)
{
    GetGLID() = Other.GetGLID();
    Other.GetGLID() = 0;
}

vertex_array &vertex_array::operator=(vertex_array &&Other)
{
    if (this != &Other)
    {
        Release();
        std::swap(GetGLID(), Other.GetGLID());
    }
    return *this;
}

vertex_array::~vertex_array()
{
    GLCall(glDeleteVertexArrays(1, &GetGLID()));
    GetGLID() = 0;
}

void vertex_array::Bind() const
{
    GLCall(glBindVertexArray(GetGLID()));
}

void vertex_array::Unbind() const
{
    GLCall(glBindVertexArray(0));
}

void vertex_array::Release()
{
    GLCall(glDeleteVertexArrays(1, &GetGLID()));
    GetGLID() = 0;
}

void vertex_array::BindVertexBuffer(uint32 ID, uint32 Slot)
{
    GLCall(glVertexArrayVertexBuffer(GetGLID(), Slot, ID, 0, 8 * sizeof(float)));
}

void vertex_array::BindIndexBuffer(uint32 ID)
{
    GLCall(glVertexArrayElementBuffer(GetGLID(), ID));
}