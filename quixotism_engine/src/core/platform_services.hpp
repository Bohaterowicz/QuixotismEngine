#pragma once
#include <memory>

#include "quixotism_c.hpp"

namespace quixotism {

struct ReadFileResult {
  std::unique_ptr<u8[]> data = nullptr;
  size_t size = 0;
};

struct FileMetadata {
  bool found;
  u64 last_write_time;
};

using ReadFileFunPtr = ReadFileResult (*)(const char *);
using WriteFileFunPtr = bool (*)(const char *, u8 *, size_t);
using GetFileMetadataFunPtr = FileMetadata (*)(const char *);
using GetWorldTimestampFunPtr = u64 (*)();

struct PlatformServices {
  ReadFileFunPtr read_file;
  WriteFileFunPtr write_file;
  GetFileMetadataFunPtr get_file_metadata;
  GetWorldTimestampFunPtr get_world_timestamp;
};

}  // namespace quixotism