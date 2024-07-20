#include "obj_parser.hpp"

#include <cctype>
#include <charconv>
#include <vector>

namespace quixotism {

template <class VEC_TYPE>
static VEC_TYPE ParseVecDataFromString(const char *&StartP,
                                       i32 MaxPrecision = 6) {
  VEC_TYPE result;
  for (auto &P : result.e)  // NOLINT
  {
    const auto *EndP = StartP;
    while (*EndP != ' ' && *EndP != '\n') {
      ++EndP;
    }
    std::from_chars(StartP, EndP, P);
    if (*EndP == ' ') ++EndP;
    StartP = EndP;
  }
  while (*StartP != '\n') ++StartP;
  return result;
}

static void ParseFaceIndices(TriangleIndices &Indices, i32 PosIdxOffset,
                             i32 TexCoordIdxOffset, i32 NormalIdxOffset,
                             const char *&Pos) {
  i32 Index = 0;

  while (Index < 3) {
    i32 IndexVal = 0;
    while (*Pos != '/') {
      Assert(isdigit(*Pos));
      IndexVal = (IndexVal * 10) + *Pos - '0';
      ++Pos;
    }
    ++Pos;
    Indices.PosIdx.push_back(IndexVal - PosIdxOffset - 1);

    IndexVal = 0;
    while (*Pos != '/') {
      Assert(isdigit(*Pos));
      IndexVal = (IndexVal * 10) + *Pos - '0';
      ++Pos;
    }
    ++Pos;
    Indices.TexCoordIdx.push_back(IndexVal - TexCoordIdxOffset - 1);

    IndexVal = 0;
    while (*Pos != ' ') {
      Assert(isdigit(*Pos));
      IndexVal = (IndexVal * 10) + *Pos - '0';
      ++Pos;
    }
    ++Pos;
    Indices.NormalIdx.push_back(IndexVal - NormalIdxOffset - 1);
    ++Index;
  }
}

std::vector<Mesh> ParseOBJ(const void *ObjFileData, size_t FileSize) {
  std::vector<Mesh> Objects;
  // Sanity check, that we actaully got some data to parse...
  if (!ObjFileData || FileSize == 0) {
    return Objects;
  }

  auto *CurrentP = static_cast<const char *>(ObjFileData);
  auto *EndP = CurrentP + FileSize;

  i32 PosIdxOffset = 0;
  i32 NormalIdxOffset = 0;
  i32 TexCoordIdxOffset = 0;
  Mesh CurrentObject;
  bool NewObject = true;

  // keep looping until we get to the file end
  while (CurrentP != EndP) {
    // check the first character of the line
    switch (*CurrentP) {
      case 'v': {
        // got vertex data
        if (NewObject) {
          if (!CurrentObject.VertexPosData.empty()) {
            PosIdxOffset += CurrentObject.VertexPosData.size();
            NormalIdxOffset += CurrentObject.VertexNormalData.size();
            TexCoordIdxOffset += CurrentObject.VertexTexCoordData.size();
            Objects.push_back(std::move(CurrentObject));
            CurrentObject = Mesh();
          }
          CurrentObject.VertexPosData.reserve(1024);
          CurrentObject.VertexNormalData.reserve(1024);
          CurrentObject.VertexTexCoordData.reserve(1024);
          CurrentObject.VertexTriangleIndicies.PosIdx.reserve(1024);
          CurrentObject.VertexTriangleIndicies.NormalIdx.reserve(1024);
          CurrentObject.VertexTriangleIndicies.TexCoordIdx.reserve(1024);
          NewObject = false;
        }
        // advance to next character
        ++CurrentP;
        switch (*CurrentP) {
          case ' ': {
            // got vertex positions
            // skip past white-spcae
            CurrentP += 2;
            CurrentObject.VertexPosData.emplace_back(
                ParseVecDataFromString<VertexPos>(CurrentP));
          } break;
          case 't': {
            // got vertex textuure coordinates
            CurrentP += 2;
            CurrentObject.VertexTexCoordData.emplace_back(
                ParseVecDataFromString<VertexTexCoords>(CurrentP));
          } break;
          case 'n': {
            // got vertex normals
            CurrentP += 2;
            CurrentObject.VertexNormalData.emplace_back(
                ParseVecDataFromString<VertexNormal>(CurrentP));
          } break;
          default: {
            Assert(!"IMPOSSIBLE CODE PATH");
          } break;
        }
      } break;
      case 'f': {
        // got face data
        CurrentP += 2;
        ParseFaceIndices(CurrentObject.VertexTriangleIndicies, PosIdxOffset,
                         TexCoordIdxOffset, NormalIdxOffset, CurrentP);
      } break;
      case 'o': {
        // got object name
        NewObject = true;
        CurrentP += 2;
        auto NameEnd = CurrentP;
        while (*NameEnd != '\n') {
          ++NameEnd;
        }
        CurrentObject.ObjectName = std::string(CurrentP, NameEnd);
        CurrentP = NameEnd;
      } break;
      default: {
        // first character of the line did not match any exected character, skip
        // to the next line character
        while (*CurrentP != '\n') {
          ++CurrentP;
        }
      } break;
    }
    // advance to next character
    ++CurrentP;
  }
  Objects.push_back(std::move(CurrentObject));

  return Objects;
}

}  // namespace quixotism