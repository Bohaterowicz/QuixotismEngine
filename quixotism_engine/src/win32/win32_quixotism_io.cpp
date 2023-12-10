#include "win32_quixotism_io.hpp"

#include <memory>

#include "Windows.h"
#include "quixotism_c.hpp"

namespace quixotism::win32 {

ReadFileResult Win32ReadFile(const char *file_path) {
  ReadFileResult result = {};
  // Open handle to file as read-only
  HANDLE file_handle =  // NOLINT
      CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                  OPEN_EXISTING, 0, nullptr);
  if (file_handle != INVALID_HANDLE_VALUE)  // NOLINT
  {
    // Get size of the file
    LARGE_INTEGER file_size;
    if (GetFileSizeEx(file_handle, &file_size)) {
      // Truncate the 64bit file size into 32bit
      // This is because ReadFile call can only take a DWORD (32bit) size to
      // read, which would require multiple reads for files bigger than maximum
      // 32bit number. Since it is a debug call, we should never read files this
      // big (>4GB) using this call
      u32 file_size32 = SafeU64ToU32(file_size.QuadPart);
      // Allocate buffer needed to store the file contents
      result.data = std::make_unique<u8[]>(file_size32);
      if (result.data) {
        // Read the content of the file into the buffer
        DWORD bytes_read = 0;
        if (ReadFile(file_handle, result.data.get(), file_size32, &bytes_read,
                     nullptr) &&
            (bytes_read == file_size32)) {
          result.size = file_size32;
        } else {
          // if reading file failed
          result = {};
        }
      }
    }
    // Close the handle to the file
    CloseHandle(file_handle);
  }
  return result;
}

FileMetadata Win32GetFileMetadata(const char *file_path) {
  FileMetadata result = {};
  // Find file and get its data
  WIN32_FIND_DATA find_data;
  HANDLE find_handle = FindFirstFileA(file_path, &find_data);
  if (find_handle != INVALID_HANDLE_VALUE) {
    FindClose(find_handle);
    result.found = true;
    result.last_write_time =
        ULARGE_INTEGER{find_data.ftLastWriteTime.dwLowDateTime,
                       find_data.ftLastWriteTime.dwHighDateTime}
            .QuadPart;
  }

  return result;
}

bool Win32WriteFile(const char *file_path, u8 *data, size_t size) {
  bool result = false;
  HANDLE file_handle =
      CreateFileA(file_path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
  if (file_handle != INVALID_HANDLE_VALUE) {
    DWORD bytes_written;
    if (WriteFile(file_handle, data, size, &bytes_written, nullptr)) {
      result = (bytes_written == size);
    }
    CloseHandle(file_handle);
  }
  return result;
}

}  // namespace quixotism::win32