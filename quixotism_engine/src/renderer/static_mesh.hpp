#pragma once
#include "index_buffer.hpp"
#include "mesh_data.hpp"
#include "quixotism_c.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"

#include <memory>

class static_mesh
{
  public:
    static_mesh();
    explicit static_mesh(std::shared_ptr<vertex_buffer> &VBO,
                         std::shared_ptr<index_buffer> &IBO, std::shared_ptr<vertex_array> &VAO);

    _NODISCARD const vertex_array &GetVertexArray() const
    {
        return *VertexArray;
    }

    _NODISCARD const vertex_buffer &GetVertexBuffer() const
    {
        return *VertexBuffer;
    }

    _NODISCARD const index_buffer &GetIndexBuffer() const
    {
        return *IndexBuffer;
    }

    void Draw();

  private:
    std::shared_ptr<vertex_buffer> VertexBuffer = nullptr;
    std::shared_ptr<index_buffer> IndexBuffer = nullptr;
    std::shared_ptr<vertex_array> VertexArray = nullptr;
};