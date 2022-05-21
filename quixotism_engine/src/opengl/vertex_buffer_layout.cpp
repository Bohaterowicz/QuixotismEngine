#include "vertex_buffer_layout.hpp"
#include <GL/glew.h>

void vertex_buffer_layout::AddLayoutElementF(uint32 Count, const char *ElementName, bool32 Normalize)
{
    AddLayoutElement(Count, ElementName, Normalize, GL_FLOAT, sizeof(GLfloat));
}

void vertex_buffer_layout::AddLayoutElementUI(uint32 Count, const char *ElementName, bool32 Normalize)
{
    AddLayoutElement(Count, ElementName, Normalize, GL_UNSIGNED_INT, sizeof(GLuint));
}

void vertex_buffer_layout::AddLayoutElementUB(uint32 Count, const char *ElementName, bool32 Normalize)
{
    AddLayoutElement(Count, ElementName, Normalize, GL_BYTE, sizeof(GLubyte));
}

void vertex_buffer_layout::AddLayoutElement(uint32 Count, const char *Name, bool32 Normalize, uint32 Type,
                                            size TypeSize)
{
    LayoutElements.push_back({Name, Type, TypeSize, Count, Stride, Normalize});
    Stride += TypeSize * Count;
}