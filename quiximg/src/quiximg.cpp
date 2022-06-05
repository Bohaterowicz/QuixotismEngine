#include "quiximg.hpp"

namespace qimg
{

loaded_img LoadImage(uint8 *ImageData, size DataSize)
{
    loaded_img Result;
    if (ImageData && DataSize > 0)
    {
        auto Type = CheckImageType(ImageData);

        switch (Type)
        {
        case image_type::BMP: {
            auto BMPResult = LoadBMP(ImageData, DataSize);
            Result.Data = std::move(BMPResult.Data);
            Result.Width = BMPResult.Info.Width;
            Result.Height = BMPResult.Info.Height;
            if (BMPResult.Info.BitCount == 24 && BMPResult.Info.AlphaMask == 0xff000000)
            {
                Result.NChannels = 3;
            }
            else
            {
                Result.NChannels = BMPResult.Info.AlphaMask ? 4 : 3;
            }
        }
        break;
        default: {
        }
        }
    }
    else
    {
        DEBUG_OUT("Image data or size is null, could not load image...");
    }
    return Result;
}

bool32 IsPNG(uint8 *Data)
{
    return false;
}

bool32 IsBMP(uint8 *Data)
{
    uint16 BMPSignature = 0x4d42;
    auto *DataSignature = reinterpret_cast<uint16 *>(Data);
    if (BMPSignature == *DataSignature)
    {
        return true;
    }
    return false;
}

bool32 IsDDS(uint8 *Data)
{
    return false;
}

image_type CheckImageType(uint8 *Data)
{
    image_type Type = image_type::UNSUPPORTED;
    if (IsPNG(Data) == TRUE)
    {
        Type = image_type::PNG;
    }
    else if (IsBMP(Data) == TRUE)
    {
        Type = image_type::BMP;
    }
    else if (IsDDS(Data) == TRUE)
    {
        Type = image_type::DDS;
    }
    return Type;
}

} // namespace qimg