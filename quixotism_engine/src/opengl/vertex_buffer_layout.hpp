#pragma once
#include "quixotism_c.hpp"
#include <string>
#include <vector>

struct layout_element
{
    const char *Name = "";
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
    void AddLayoutElementF(uint32 Count, const char *ElementName, bool32 Normalize);
    void AddLayoutElementUI(uint32 Count, const char *ElementName, bool32 Normalize);
    void AddLayoutElementUB(uint32 Count, const char *ElementName, bool32 Normalize);

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

    void AddLayoutElement(uint32 Count, const char *Name, bool32 Normalize, uint32 Type, size TypeSize);
};