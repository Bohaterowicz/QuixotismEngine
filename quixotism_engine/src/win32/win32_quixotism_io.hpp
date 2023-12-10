#pragma once
#include "core/platform_services.hpp"

namespace quixotism::win32 {

[[nodiscard]] ReadFileResult Win32ReadFile(const char *file_path);

[[nodiscard]] bool Win32WriteFile(const char *file_path, u8 *data, size_t size);

[[nodiscard]] FileMetadata Win32GetFileMetadata(const char *file_path);

}  // namespace quixotism::win32
