#pragma once
#include "Windows.h"
#include "quixotism_c.hpp"
#include "quixotism_math.hpp"
#include "quixotism_platform_services.hpp"

_NODISCARD read_file_result Win32ReadFile(const std::string &FilePath)
{
    read_file_result Result = {};
    // Open handle to file as read-only
    HANDLE FileHandle = // NOLINT
        CreateFileA(FilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (FileHandle != INVALID_HANDLE_VALUE) // NOLINT
    {
        // Get size of the file
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(FileHandle, &FileSize))
        {
            // Truncate the 64bit file size into 32bit
            // This is because ReadFile call can only take a DWORD (32bit) size to read, which would require multiple
            // reads for files bigger than maximum 32bit number. Since it is a debug call, we should never read files
            // this big (>4GB) using this call
            uint32 FileSize32 = SafeTruncateUint64(FileSize.QuadPart);
            // Allocate buffer needed to store the file contents
            Result.Content = std::make_unique<uint8[]>(FileSize32);
            if (Result.Content)
            {
                // Read the content of the file into the buffer
                DWORD BytesRead = 0;
                if (ReadFile(FileHandle, Result.Content.get(), FileSize32, &BytesRead, nullptr) &&
                    (BytesRead == FileSize32))
                {
                    Result.Size = FileSize32;
                }
                else
                {
                    // if reading file failed
                    Result = {};
                }
            }
        }
        // Close the handle to the file
        CloseHandle(FileHandle);
    }
    return Result;
}