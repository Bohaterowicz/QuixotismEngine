#include "mesh_data.hpp"

std::unique_ptr<real32[]> SerializeDataByLayout(vertex_buffer_layout &Layout, qmesh::object_mesh *Data, size *Size)
{
    size Count = 0;
    for (const auto &Element : Layout.GetLayout())
    {
        Count += Element.Count;
    }
    Count *= (Data->Faces.size() * 3); // mul by 3 because triangles have 3 verticies
    auto Buffer = std::make_unique<real32[]>(Count);

    size BufferOffset = 0;
    for (auto &Face : Data->Faces)
    {
        for (int32 TriIndex : {0, 1, 2})
        {
            for (const auto &Element : Layout.GetLayout())
            {
                if (Element.ElementType == layout_element_type::POSITION)
                {
                    Assert(!Data->VertexPositions.empty());
                    std::memcpy(Buffer.get() + BufferOffset, &Data->VertexPositions[Face.PosIdx[TriIndex]],
                                3 * sizeof(real32));
                    BufferOffset += 3;
                }
                else if (Element.ElementType == layout_element_type::NORMAL)
                {
                    Assert(!Data->VertexNormals.empty());
                    std::memcpy(Buffer.get() + BufferOffset, &Data->VertexNormals[Face.NormalIdx[TriIndex]],
                                3 * sizeof(real32));
                    BufferOffset += 3;
                }
                else if (Element.ElementType == layout_element_type::TEXCOORD)
                {
                    Assert(!Data->TextureCoordinates.empty());
                    std::memcpy(Buffer.get() + BufferOffset, &Data->TextureCoordinates[Face.TexCoordIdx[TriIndex]],
                                3 * sizeof(real32));
                    BufferOffset += 3;
                }
            }
        }
    }

    *Size = Count;
    return std::move(Buffer);
}

std::unique_ptr<uint32[]> GenerateIndexBuffer(size TriangleCount)
{
    auto Buffer = std::make_unique<uint32[]>(TriangleCount * 3);
    auto *BufferPtr = Buffer.get();

    for (uint32 Index = 0; Index < TriangleCount * 3; ++Index)
    {
        BufferPtr[Index] = Index;
    }

    return std::move(Buffer);
}