#pragma once
#include "gl_buffer.hpp"
#include "quixotism_c.hpp"

class vertex_buffer : public gl_buffer
{
  public:
    vertex_buffer();

    vertex_buffer(const vertex_buffer &) = delete;
    vertex_buffer(vertex_buffer &&Other);

    vertex_buffer &operator=(const vertex_buffer &) = delete;
    vertex_buffer &operator=(vertex_buffer &&Other);

    ~vertex_buffer() override = default;
};