#pragma once
#include "quixotism_c.hpp"
#include <memory>
#include <string>
#include <vector>

namespace qobj
{

enum class error
{
    NO_ERROR = 0,
    INDEX_OUT_OF_BOUNDS,
    MALFORMED_INDEX_REGION,
};

struct vertex_position
{
    union {
        real32 E[3];
        struct
        {
            real32 X, Y, Z;
        };
    };
};

struct texture_coordinates
{
    union {
        real32 E[3];
        struct
        {
            real32 U, V, W;
        };
    };
};

struct object_indicies
{
    int32 I[3];
};

struct object_mesh
{
    std::vector<vertex_position> VertexPositions;
    std::vector<texture_coordinates> TextureCoordinates;
    std::vector<object_indicies> Indicies;
    std::string ObjectName{};
    bool32 NotTriangulated = false;
};

struct obj_data
{
    std::vector<object_mesh> Objects;
};

std::unique_ptr<obj_data> ParseOBJ(const void *ObjFileData, size FileSize);
} // namespace qobj