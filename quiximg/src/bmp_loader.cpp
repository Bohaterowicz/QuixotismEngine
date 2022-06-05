#include "bmp_loader.hpp"
#include "bit_ops.hpp"

namespace qimg
{
load_bmp_result LoadBMP(uint8 *Data, size Size)
{
    load_bmp_result Result;

    auto *FileHeader = reinterpret_cast<bmp_file_header *>(Data);
    Assert(Size == FileHeader->Size);

    // Get pointer to the data section (where the pixel data resides)
    auto *PixelData = Data + FileHeader->DataOffset;

    // Advance pointer past header struct
    Data += sizeof(bmp_file_header);
    uint32 InfoHeaderSize = *(reinterpret_cast<uint32 *>(Data));

    // We copy the header data into our header, unsued parameters will be 0;
    std::memcpy(&Result.Info, Data, InfoHeaderSize);

    if (Result.Info.ImageBufferSize > 0)
    {
        // We allocate memory needed for our pixel buffer
        Result.Data = std::make_unique<uint8[]>(Result.Info.ImageBufferSize);
        // We copy pixel data from file to our pixel buffer;
        std::memcpy(Result.Data.get(), PixelData, Result.Info.ImageBufferSize);

        uint32 RedMask = Result.Info.RedMask;
        uint32 GreenMask = Result.Info.GreenMask;
        uint32 BlueMask = Result.Info.BlueMask;
        uint32 AlphaMask = ~(RedMask | GreenMask | BlueMask);

        bit_scan_result RedScan = BitScanForward(RedMask);
        bit_scan_result GreenScan = BitScanForward(GreenMask);
        bit_scan_result BlueScan = BitScanForward(BlueMask);
        bit_scan_result AlphaScan = BitScanForward(AlphaMask);

        Assert(RedScan.Found);
        Assert(GreenScan.Found);
        Assert(BlueScan.Found);
        Assert(AlphaScan.Found);

        int32 AlphaShift = 24 - static_cast<int32>(AlphaScan.Index);
        int32 RedShift = 16 - static_cast<int32>(RedScan.Index);
        int32 GreenShift = 8 - static_cast<int32>(GreenScan.Index);
        int32 BlueShift = 0 - static_cast<int32>(BlueScan.Index);

        // If the file is not in BGRA format, then atleast one of the shifts will be non-zero, thus if we sum all the
        // shifts and the result is not zero, that means we must shift the pixls in order to get BGRA format!
        if ((AlphaShift + RedShift + GreenShift + BlueShift) != 0)
        {
            auto *SourceDest = reinterpret_cast<uint32 *>(Result.Data.get());
            for (int32 ImgY = 0; ImgY < Result.Info.Height; ++ImgY)
            {
                for (int32 ImgX = 0; ImgX < Result.Info.Width; ++ImgX)
                {
                    uint32 Color = *SourceDest;
#if 0
                *SourceDest =
                    ((((Color >> AlphaShift.Index) & 0xFF) << 24) | (((Color >> RedShift.Index) & 0xFF) << 16) |
                     (((Color >> GreenShift.Index) & 0xFF) << 8) | (((Color >> BlueShift.Index) & 0xFF) << 0));
#else
                    *SourceDest =
                        (BitRotateLeft(Color & AlphaMask, AlphaShift) | BitRotateLeft(Color & RedMask, RedShift) |
                         BitRotateLeft(Color & GreenMask, GreenShift) | BitRotateLeft(Color & BlueMask, BlueShift));
#endif
                }
            }
        }
    }

    return Result;
}
} // namespace qimg