#include "quixobj.hpp"
#include "debug_out.hpp"
#include <charconv>
#include <istream>

namespace qobj
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

object_indicies ParseObjectIndicies(const char *&Current, error &ErrorCode)
{
    object_indicies Indicies{};
    int32 Index = 0;
    while (*Current != '\n')
    {
        if (Index > 2)
        {
            ErrorCode = error::INDEX_OUT_OF_BOUNDS;
            return object_indicies{};
        }

        const auto *NumberEnd = Current;
        while (*NumberEnd != ' ')
        {
            ++NumberEnd;
        }

        size NumberLength = NumberEnd - Current;

        switch (NumberLength)
        { // handle up to 10 digits, assume we're 32-bit
        case 10:
            Indicies.I[Index] += (Current[NumberLength - 10] - '0') * 1000000000;
        case 9:
            Indicies.I[Index] += (Current[NumberLength - 9] - '0') * 100000000;
        case 8:
            Indicies.I[Index] += (Current[NumberLength - 8] - '0') * 10000000;
        case 7:
            Indicies.I[Index] += (Current[NumberLength - 7] - '0') * 1000000;
        case 6:
            Indicies.I[Index] += (Current[NumberLength - 6] - '0') * 100000;
        case 5:
            Indicies.I[Index] += (Current[NumberLength - 5] - '0') * 10000;
        case 4:
            Indicies.I[Index] += (Current[NumberLength - 4] - '0') * 1000;
        case 3:
            Indicies.I[Index] += (Current[NumberLength - 3] - '0') * 100;
        case 2:
            Indicies.I[Index] += (Current[NumberLength - 2] - '0') * 10;
        case 1:
            Indicies.I[Index] += (Current[NumberLength - 1] - '0');
            break;
        default:
            ErrorCode = error::MALFORMED_INDEX_REGION;
            return object_indicies{};
        }
        ++Index;
        SkipWhitespaces(NumberEnd);
        Current = NumberEnd;
    }
    ++Current;
    return Indicies;
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
            CurrentObject.Indicies.push_back(ParseObjectIndicies(Current, ErrorCode));
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

    if (ObjData)
    {
        DEBUG_OUT("Failed to parse OBJ file...");
    }

    return ObjData;
}
} // namespace qobj