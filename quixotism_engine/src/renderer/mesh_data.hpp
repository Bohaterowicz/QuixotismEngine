#pragma once
#include "quixmesh/src/quixmesh.hpp"
#include "quixotism_c.hpp"
#include "vertex_buffer_layout.hpp"

#include <memory>
#include <vector>

std::unique_ptr<real32[]> SerializeDataByLayout(vertex_buffer_layout &Layout, qmesh::object_mesh *Data, size *Size);
std::unique_ptr<uint32[]> GenerateIndexBuffer(size TriangleCount);
