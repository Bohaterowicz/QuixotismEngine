#pragma once
#include "quixotism_c.hpp"
#include <array>

const uint32 MAX_SUPPORTED_CONTROLLERS = 5;
const uint32 SUPPORTED_CONTROLLERS_BUTTONS_COUNT = 12;
const uint32 SUPPORTED_MOUSE_BUTTON_COUNT = 5;

struct button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
};

struct controller_input
{
    bool32 IsConnected;
    bool32 IsAnalog;

    real32 StickAverageX;
    real32 StickAverageY;

    union {
        button_state Buttons[SUPPORTED_CONTROLLERS_BUTTONS_COUNT];
        struct
        {
            button_state Up;
            button_state Down;
            button_state Left;
            button_state Right;
            button_state A;
            button_state B;
            button_state X;
            button_state Y;
            button_state LeftShoulder;
            button_state RightShoulder;
            button_state Start;
            button_state Back;

            // IMPORTANT: All buttons must be added before this line!
            button_state _END;
        };
    };
};

struct engine_input
{
    std::array<button_state, SUPPORTED_MOUSE_BUTTON_COUNT> MouseButtons;

    int32 MouseX, MouseY, MouseZ;
    real32 TimeStep;
    controller_input Controllers[MAX_SUPPORTED_CONTROLLERS];
};

inline controller_input *GetController(engine_input *Input, uint32 Index)
{
    Assert(Index < ArrayCount(Input->Controllers));
    return &Input->Controllers[Index];
}

/**
 * @brief Returns keybaord controller. Keyboard controller is always at index 0 in Controllers array!
 *
 * @param engine_input* Input struct
 * @return controller_input* Keybaord Controller
 */
inline controller_input *GetKeybaordController(engine_input *Input)
{
    controller_input *KeybaordController = GetController(Input, 0);
    return KeybaordController;
}