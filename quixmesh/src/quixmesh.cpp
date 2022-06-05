#include "quixmesh.hpp"
#include "debug_out.hpp"
#include <charconv>
#include <istream>

namespace qmesh
{
inline void SkipLine(const char *&CurrentPtr)
{
    while (*CurrentPtr++ != '\n')
    {
    }
}

inline void SkipWhitespaces(const char *&CurrentPtr)
{
    while (*CurrentPtr == ' ' || *CurrentPtr == '\r')
    {
        ++CurrentPtr;
    }
}

inline void ReverseSkipWhitespaces(const char *&CurrentPtr)
{
    while (*(CurrentPtr - 1) == ' ' || *(CurrentPtr - 1) == '\r')
    {
        --CurrentPtr;
    }
}

object_face ParseObjectFaces(const char *&Current, error &ErrorCode)
{
    object_face Face{};
    int32 Index = 0;
    while (*Current != '\n')
    {
        if (Index > 2)
        {
            ErrorCode = error::INDEX_OUT_OF_BOUNDS;
            return object_face{};
        }

        int32 IdxType = 0;
        while (*Current != ' ')
        {
            const auto *NumberEnd = Current;
            while (*NumberEnd != '/' && *NumberEnd != ' ')
            {
                ++NumberEnd;
            }

            size NumberLength = NumberEnd - Current;

            uint32 VertIndex = 0;

            switch (NumberLength)
            { // handle up to 10 digits, assume we're 32-bit
            case 10:
                VertIndex += (Current[NumberLength - 10] - '0') * 1000000000;
            case 9:
                VertIndex += (Current[NumberLength - 9] - '0') * 100000000;
            case 8:
                VertIndex += (Current[NumberLength - 8] - '0') * 10000000;
            case 7:
                VertIndex += (Current[NumberLength - 7] - '0') * 1000000;
            case 6:
                VertIndex += (Current[NumberLength - 6] - '0') * 100000;
            case 5:
                VertIndex += (Current[NumberLength - 5] - '0') * 10000;
            case 4:
                VertIndex += (Current[NumberLength - 4] - '0') * 1000;
            case 3:
                VertIndex += (Current[NumberLength - 3] - '0') * 100;
            case 2:
                VertIndex += (Current[NumberLength - 2] - '0') * 10;
            case 1:
                VertIndex += (Current[NumberLength - 1] - '0');
                if (IdxType == 0)
                {
                    Face.PosIdx[Index] = VertIndex - 1;
                }
                else if (IdxType == 1)
                {
                    Face.TexCoordIdx[Index] = VertIndex - 1;
                }
                else if (IdxType == 2)
                {
                    Face.NormalIdx[Index] = VertIndex - 1;
                }
                break;
            default:
                break;
            }
            ++IdxType;
            if (*NumberEnd == '/')
            {
                ++NumberEnd;
            }
            Current = NumberEnd;
        }
        SkipWhitespaces(Current);
        ++Index;
    }
    ++Current;
    return Face;
}

texture_coordinates ParseTextureCoords(const char *Current)
{
    texture_coordinates TexCoord{};

    for (auto &Coord : TexCoord.E)
    {
        const auto *NumberEnd = Current;
        while (*NumberEnd != ' ' && *NumberEnd != '\n')
        {
            ++NumberEnd;
        }
        std::from_chars(Current, NumberEnd, Coord);
        Current = ++NumberEnd;
    }
    return TexCoord;
}

vertex_position ParseVertexPosition(const char *&Current)
{
    vertex_position Pos{};

    for (auto &P : Pos.E)
    {
        const auto *NumberEnd = Current;
        while (*NumberEnd != ' ' && *NumberEnd != '\n')
        {
            ++NumberEnd;
        }
        std::from_chars(Current, NumberEnd, P);
        Current = ++NumberEnd;
    }
    return Pos;
}

std::unique_ptr<obj_data> ParseObjects(const char *Start, const char *End, error &ErrorCode)
{
    ErrorCode = error::NO_ERROR;
    auto ObjData = std::make_unique<obj_data>();
    const auto *Current = Start;

    object_mesh CurrentObject{};
    bool32 NewObject = false;

    while (Current != End)
    {
        // IMPORTANT(albertml): Each of the non default cases must advance the current pointer to the next line!
        switch (*Current)
        {
        case 'v': {
            switch (*(++Current))
            {
            case ' ':
            case '\t':
            case '\r': {
                if (NewObject)
                {
                    ObjData->Objects.push_back(CurrentObject);
                    CurrentObject = {};
                }
                SkipWhitespaces(++Current);
                CurrentObject.VertexPositions.push_back(ParseVertexPosition(Current));
            }
            break;
            case 'n': {
            }
            break;
            case 't': {
                SkipWhitespaces(++Current);
                CurrentObject.TextureCoordinates.push_back(ParseTextureCoords(Current));
            }
            break;
            }
        }
        break;
        case 'f': {
            NewObject = true;
            SkipWhitespaces(++Current);
            CurrentObject.Faces.push_back(ParseObjectFaces(Current, ErrorCode));
            if (ErrorCode != error::NO_ERROR)
            {
                return nullptr;
            }
        }
        break;
        case 'o': {
            SkipWhitespaces(++Current);
            const auto *LineEnd = Current;
            SkipLine(LineEnd);
            ReverseSkipWhitespaces(--LineEnd);
            CurrentObject.ObjectName = std::string{Current, LineEnd};
            Current = LineEnd;
        }
        break;
        default:
            SkipLine(Current);
        }
    }

    ObjData->Objects.push_back(CurrentObject);

    return ObjData;
}

std::unique_ptr<obj_data> ParseOBJ(const void *ObjFileData, size FileSize)

{
    // Sanity check, that we actaully got some data to parse...
    if (!ObjFileData || FileSize == 0)
    {
        return nullptr;
    }

    const auto *StartPtr = static_cast<const char *>(ObjFileData);

    // Check that the file ends with \n
    if (*(StartPtr + FileSize - 1) != '\n')
    {
        return nullptr;
    }

    const auto *EndPtr = StartPtr + FileSize;
    error ParsingError = error::NO_ERROR;
    auto ObjData = ParseObjects(StartPtr, EndPtr, ParsingError);

    if (!ObjData)
    {
        DEBUG_OUT("Failed to parse OBJ file...");
    }

    return ObjData;
}
} // namespace qmesh