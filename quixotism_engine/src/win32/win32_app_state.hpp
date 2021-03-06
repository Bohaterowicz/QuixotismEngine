#pragma once
#include "quixotism_c.hpp"
#include <Windows.h>
#include <utility>

class win32_app_state
{
  public:
    explicit win32_app_state(int32 ClientWidth, int32 ClientHeight)
        : Width{ClientWidth}, Height{ClientHeight}, Running{FALSE}, Paused{FALSE}
    {
    }

    /**
     * @brief Get the Client Dimensions of the window, note that client area means area of the window excluding window
     * borders. This is exactly the canvas size.
     *
     * @return std::pair<int32, int32> as [ClientWidth, ClientHeight]
     */
    [[nodiscard]] auto GetClientDimensions() const
    {
        return std::make_pair(Width, Height);
    }

    void SetClientDimensions(int32 NewWidth, int32 NewHeight) noexcept
    {
        Width = NewWidth;
        Height = NewHeight;
    }

    /**
     * @brief Check if the app is currently running
     *
     * @return bool32 - TRUE if app is running, FALSE if not.
     */
    [[nodiscard]] auto IsRunning() const
    {
        return Running;
    }

    /**
     * @brief Stop the app, this will effectively close the app in a gracefull manner.
     *
     */
    void Stop()
    {
        Running = FALSE;
    }

    /**
     * @brief Start the app (main app loop).
     *
     */
    void Start()
    {
        Running = TRUE;
    }

    /**
     * @brief Pauses the app.
     *
     */
    void Pause()
    {
        Paused = TRUE;
    }

    /**
     * @brief Unpauses the app.
     *
     */
    void UnPause()
    {
        Paused = FALSE;
    }

    [[nodiscard]] auto const &GetPreviousCursorClip() const noexcept
    {
        return PreviousCursorClip;
    }

    void EnableCursorTrap(RECT NewWindowCursorClip, RECT OldCursorClip) noexcept
    {
        WindowCursorClip = NewWindowCursorClip;
        PreviousCursorClip = OldCursorClip;
        TrapCursor = TRUE;
    }

    void EnableInputGathering()
    {
        GatherInput = true;
    }

    void DisableInputGathering()
    {
        GatherInput = false;
    }

    _NODISCARD bool32 IsGatheringInput() const
    {
        return GatherInput;
    }

    void DisableCursorTrap() noexcept
    {
        TrapCursor = FALSE;
    }

    _NODISCARD auto IsCursorTrapped() const noexcept
    {
        return TrapCursor;
    }

    _NODISCARD auto IsCursorHidden() const noexcept
    {
        return CursorHidden;
    }

    void ToggleCursorHidden() noexcept
    {
        CursorHidden = !CursorHidden;
    }

  private:
    // Client area width (excluding window border, only working area)
    int32 Width;
    // Client area Height (excluding window border, only working area)
    int32 Height;

    // Variable controlling if the app has been started (main app loop)
    bool32 Running = false;

    bool32 Paused = false;

    bool32 GatherInput = true;

    // Cursor trap variables
    bool32 TrapCursor = FALSE;
    RECT PreviousCursorClip = {};
    RECT WindowCursorClip = {};

    bool32 CursorHidden = false;
};