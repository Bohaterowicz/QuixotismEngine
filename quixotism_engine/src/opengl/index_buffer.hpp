#pragma once
#include "gl_buffer.hpp"
#include "quixotism_c.hpp"

class index_buffer : public gl_buffer
{
  public:
    index_buffer();

    index_buffer(const index_buffer &) = delete;
    index_buffer(index_buffer &&Other);

    index_buffer &operator=(const index_buffer &) = delete;
    index_buffer &operator=(index_buffer &&Other);

    ~index_buffer() override = default;
};