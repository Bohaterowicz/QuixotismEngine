#pragma once
#include "quixotism_c.hpp"
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

  private:
    // Client area width (excluding window border, only working area)
    int32 Width;
    // Client area Height (excluding window border, only working area)
    int32 Height;

    // Variable controlling if the app has been started (main app loop)
    bool32 Running;

    bool32 Paused;
};