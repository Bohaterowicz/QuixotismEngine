#pragma once
#include "quixotism_c.hpp"
#include <array>

const uint32 MAX_SUPPORTED_CONTROLLERS = 5;
const uint32 SUPPORTED_CONTROLLERS_BUTTONS_COUNT = 14;
const uint32 SUPPORTED_MOUSE_BUTTON_COUNT = 5;

// IMPORTANT: the value of each of the button types correspands to index in the MouseButtons array, thus the values MUST
// be continous, and should never exceed the size of MouseButton array.
//  Thus when adding support for new mouse buttons, one must add the enum here we correct index corresponding to
//  MouseButton index of the new button!
enum class mouse_button_type : uint8
{
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2,
    X1 = 3,
    X2 = 4
};

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
            button_state Forward;
            button_state Backward;
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

class engine_input
{
  public:
    engine_input() = default;
    explicit engine_input(real32 TargetTimeStep) noexcept : TimeStep{TargetTimeStep}
    {
    }

    auto &GetController(uint32 Index) noexcept
    {
        Assert(Index < Controllers.size());
        return Controllers[Index];
    }

    /**
     * @brief Returns keybaord controller. Keyboard controller is always at index 0 in Controllers array!
     *
     * @param engine_input* Input struct
     * @return controller_input* Keybaord Controller
     */
    auto &GetKeybaordController() noexcept
    {
        auto &KeyboardController = GetController(0);
        return KeyboardController;
    }

    auto &GetMouseButtonState(mouse_button_type ButtonType)
    {
        Assert(static_cast<uint8>(ButtonType) < MouseButtons.size());
        return MouseButtons[static_cast<uint8>(ButtonType)];
    }

    [[nodiscard]] auto GetTimeStep() const noexcept
    {
        return TimeStep;
    }

    void SetTimeStep(real32 _TimeStep) noexcept
    {
        TimeStep = _TimeStep;
    }

    void SetCursorPositionChange(real32 DeltaX, real32 DeltaY) noexcept
    {
        MouseDeltaX = DeltaX;
        MouseDeltaY = DeltaY;
    }

  private:
    std::array<button_state, SUPPORTED_MOUSE_BUTTON_COUNT> MouseButtons = {};
    std::array<controller_input, MAX_SUPPORTED_CONTROLLERS> Controllers = {};

    real32 MouseDeltaX = 0;
    real32 MouseDeltaY = 0;
    real32 TimeStep = 0.0F;
};