#pragma once
#include "bmp_loader.hpp"
#include "quixotism_c.hpp"
#include <memory>

namespace qimg
{

#pragma pack(push, 1)
struct bmp_file_header
{
    uint16 TypeSignature;
    uint32 Size;
    uint16 Reserved1;
    uint16 Reserved2;
    uint32 DataOffset;
};

struct CIEXYZ
{
    int32 X;
    int32 Y;
    int32 Z;
};

struct CIEXYZ_triple
{
    CIEXYZ Red;
    CIEXYZ Green;
    CIEXYZ Blue;
};

struct bmp_info
{
    uint32 Size;
    int32 Width;
    int32 Height;
    uint16 Planes;
    uint16 BitCount;
    uint32 Compression;
    uint32 ImageBufferSize;
    int32 PixelsPerMeterX;
    int32 PixelsPerMeterY;
    uint32 ColorsUsed;
    uint32 ColorsImportant;
    uint32 RedMask;
    uint32 GreenMask;
    uint32 BlueMask;
    uint32 AlphaMask;
    uint32 ColorSpace;
    CIEXYZ_triple ColorEndpoints;
    uint32 RedGamma;
    uint32 GreenGamma;
    uint32 BlueGamme;
    uint32 Intent;
    uint32 ProfileData;
    uint32 ProfileSize;
    uint32 Reserved;
};
#pragma pack(pop)

struct load_bmp_result
{
    std::unique_ptr<uint8[]> Data;
    bmp_info Info{};
};

load_bmp_result LoadBMP(uint8 *Data, size Size);
} // namespace qimg