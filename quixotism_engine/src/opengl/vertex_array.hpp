#pragma once
#include "bindable.hpp"
#include "quixotism_c.hpp"

#include "index_buffer.hpp"
#include "vertex_buffer.hpp"
#include "vertex_buffer_layout.hpp"

#include <memory>

class vertex_array : public bindable
{
  public:
    vertex_array() = default;
    explicit vertex_array(vertex_buffer_layout &VBLayout);

    vertex_array(const vertex_array &) = delete;
    vertex_array(vertex_array &&Other);

    vertex_array &operator=(const vertex_array &) = delete;
    vertex_array &operator=(vertex_array &&Other);

    ~vertex_array() override;

    void Bind() const override;
    void Unbind() const override;

    void Release() override;

    void BindVertexBuffer(uint32 ID, uint32 Slot);
    void BindIndexBuffer(uint32 ID);
};