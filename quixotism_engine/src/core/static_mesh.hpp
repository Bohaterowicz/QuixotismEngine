#pragma once

#include "file_processing/obj_parser/obj_parser.hpp"
#include "quixotism_c.hpp"
#include "renderer/gl_buffer_manager.hpp"
#include "renderer/vertex_array_manager.hpp"

namespace quixotism {

class StaticMesh {
 public:
  StaticMesh() = default;
  StaticMesh(StaticMesh&& other) {
    vbo_id = other.vbo_id;
    ebo_id = other.ebo_id;
    vao_id = other.vao_id;
    bb_vbo_id = other.bb_vbo_id;
    mesh = std::move(other.mesh);
    other.vbo_id = 0;
    other.ebo_id = 0;
    other.vao_id = 0;
    other.bb_vbo_id = 0;
    other.mesh = {};
  }
  StaticMesh(Mesh&& sm) noexcept { mesh = std::move(sm); }

  StaticMesh& operator=(StaticMesh&& other) noexcept {
    vbo_id = other.vbo_id;
    ebo_id = other.ebo_id;
    vao_id = other.vao_id;
    bb_vbo_id = other.bb_vbo_id;
    mesh = std::move(other.mesh);
    other.vbo_id = 0;
    other.ebo_id = 0;
    other.vao_id = 0;
    other.bb_vbo_id = 0;
    other.mesh = {};
    return *this;
  }

  Mesh& GetMeshData() { return mesh; }

  GLBufferID vbo_id, ebo_id, bb_vbo_id;
  VertexArrayID vao_id;

 private:
  Mesh mesh;
};

}  // namespace quixotism
