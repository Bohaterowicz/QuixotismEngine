#pragma once
#include "quixotism_c.hpp"

#include <memory>
#include <vector>

struct tri_idices
{
    uint32 Position[3];
    uint32 Normal[3];
    uint32 TexCoord[3];
};

struct mesh_data
{
    std::unique_ptr<real32[]> Position;
    std::unique_ptr<real32[]> Normals;
    std::unique_ptr<real32[]> TexCoords;
    std::unique_ptr<real32[]> VertexColors;
    std::unique_ptr<uint32[]> Indices;
    uint32 IndexCount = 0;
};