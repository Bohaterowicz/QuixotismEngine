#pragma once
#include "quixotism_c.hpp"
#include <string>
#include <vector>

enum class layout_element_type
{
    UNDEFINED,
    POSITION,
    NORMAL,
    TEXCOORD,
    VERTEXCOLOR,
};

struct layout_element
{
    layout_element_type ElementType = layout_element_type::UNDEFINED;
    uint32 Type = 0;
    size TypeSize = 0;
    uint32 Count = 0;
    size Offset = 0;
    bool32 Normalize = 0;
};

class vertex_buffer_layout
{
  public:
    vertex_buffer_layout() = default;
    void AddLayoutElementF(layout_element_type ElementType, uint32 Count, bool32 Normalize);
    void AddLayoutElementUI(layout_element_type ElementType, uint32 Count, bool32 Normalize);
    void AddLayoutElementUB(layout_element_type ElementType, uint32 Count, bool32 Normalize);

    _NODISCARD const std::vector<layout_element> &GetLayout() const
    {
        return LayoutElements;
    }

    _NODISCARD size GetStride() const
    {
        return Stride;
    }

  private:
    std::vector<layout_element> LayoutElements = {};
    size Stride = 0;

    void AddLayoutElement(layout_element_type ElementType, uint32 Count, bool32 Normalize, uint32 Type, size TypeSize);
};