#pragma once
#include "bmp_loader.hpp"
#include "debug_out.hpp"
#include "quixotism_c.hpp"
#include <memory>
#include <string>

namespace qimg
{

struct loaded_img
{
    std::unique_ptr<uint8[]> Data = nullptr;
    int32 Width = 0;
    int32 Height = 0;
    int32 NChannels = 0;
};

enum class image_type : uint8
{
    UNSUPPORTED = 0,
    BMP = 1,
    PNG = 2,
    DDS = 3,
};

loaded_img LoadImage(uint8 *ImageData, size DataSize);
image_type CheckImageType(uint8 *Data);

} // namespace qimg