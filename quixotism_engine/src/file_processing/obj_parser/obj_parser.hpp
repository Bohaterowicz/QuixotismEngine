#pragma once

#include <memory>
#include <string>
#include <vector>

#include "math/qmath.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

using VertexPos = Vec3;
using VertexNormal = Vec3;
using VertexTexCoords = Vec2;

struct TriangleIndices {
  std::vector<u32> PosIdx;
  std::vector<u32> TexCoordIdx;
  std::vector<u32> NormalIdx;
};

struct AABB {
  AABB(){};
  union {
    Vec3 corners[2];
    struct {
      Vec3 min;
      Vec3 max;
    };
  };
};

struct Mesh {
  Mesh(){};
  std::string ObjectName;
  std::vector<VertexPos> VertexPosData;
  std::vector<VertexNormal> VertexNormalData;
  std::vector<VertexTexCoords> VertexTexCoordData;
  AABB bbox;
  TriangleIndices VertexTriangleIndicies;
};

std::vector<Mesh> ParseOBJ(const void *ObjFileData, size_t FileSize);
}  // namespace quixotism