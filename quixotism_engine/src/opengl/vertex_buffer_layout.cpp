#include "vertex_buffer_layout.hpp"
#include <GL/glew.h>

void vertex_buffer_layout::AddLayoutElementF(layout_element_type ElementType, uint32 Count, bool32 Normalize)
{
    AddLayoutElement(ElementType, Count, Normalize, GL_FLOAT, sizeof(GLfloat));
}

void vertex_buffer_layout::AddLayoutElementUI(layout_element_type ElementType, uint32 Count, bool32 Normalize)
{
    AddLayoutElement(ElementType, Count, Normalize, GL_UNSIGNED_INT, sizeof(GLuint));
}

void vertex_buffer_layout::AddLayoutElementUB(layout_element_type ElementType, uint32 Count, bool32 Normalize)
{
    AddLayoutElement(ElementType, Count, Normalize, GL_BYTE, sizeof(GLubyte));
}

void vertex_buffer_layout::AddLayoutElement(layout_element_type ElementType, uint32 Count, bool32 Normalize,
                                            uint32 Type, size TypeSize)
{
    LayoutElements.push_back({ElementType, Type, TypeSize, Count, Stride, Normalize});
    Stride += TypeSize * Count;
}