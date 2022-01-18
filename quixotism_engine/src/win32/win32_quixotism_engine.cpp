#include "win32_quixotism_engine.hpp"
#include "GL/glew.h"
#include "GL/wglew.h"
#include "debug_out.hpp"
#include <Windows.h>
#include <string>

#include "quixotism_engine.hpp"
#include "quixotism_input.hpp"
#include "win32_app_state.hpp"

INTERNAL auto *Win32GetAppState(HWND Window)
{
    LONG_PTR Ptr = GetWindowLongPtr(Window, GWLP_USERDATA);
    auto *WindowState = reinterpret_cast<win32_app_state *>(Ptr);
    return WindowState;
}

// Main window callback procedure
LRESULT CALLBACK Win32QuixotismEngineWindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = {};

    switch (Message)
    {

    case WM_CREATE: {
        win32_app_state *AppState;
        auto *Create = reinterpret_cast<CREATESTRUCT *>(LParam);
        AppState = reinterpret_cast<win32_app_state *>(Create->lpCreateParams);
        SetWindowLongPtr(Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(AppState));
    }
    break;

    case WM_SIZE: {
        OutputDebugStringA("WM_SIZE\n");
    }
    break;

    case WM_DESTROY: {
        auto *AppState = Win32GetAppState(Window);
        AppState->Stop();
        OutputDebugStringA("WM_DESTROY\n");
    }
    break;

    case WM_CLOSE: {
        auto *AppState = Win32GetAppState(Window);
        AppState->Stop();
        OutputDebugStringA("WM_CLOSE\n");
    }
    break;

    case WM_ACTIVATEAPP: {
        OutputDebugStringA("WM_ACTIVATEAPP\n");
    }
    break;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP: {
        Assert(!"Keyboard event came from wrong dispatch......");
    }
    break;

    default: {
        Result = DefWindowProcA(Window, Message, WParam, LParam);
    }
    break;
    }

    return Result;
}

void Win32ProcessWindowMessages(win32_app_state &AppState)
{
    // NOTE: message processing loop (non-blocking)
    MSG Message;
    while (PeekMessageA(&Message, nullptr, 0, 0, PM_REMOVE) == TRUE)
    {
        switch (Message.message)
        {
        case WM_QUIT: {
            AppState.Stop();
        }
        break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            constexpr uint32 WAS_DOWN_BIT_SHIFT = 30;
            constexpr uint32 IS_DOWN_BIT_SHIFT = 31;
            constexpr uint32 ALT_KEY_IS_DOWN_SHIFT = 29;
            auto VKCode = static_cast<uint32>(Message.wParam);
            // 30th bit in LParam indicates if the key was pressed down before (previous state)
            bool32 WasDown = ((Message.lParam & (1 << WAS_DOWN_BIT_SHIFT)) != 0);
            // 31st bit in LParam indicates if the key is currently down (current state)
            bool32 IsDown = ((Message.lParam & (1 << IS_DOWN_BIT_SHIFT)) == 0);

            bool AltKeyWasDown = ((Message.lParam & (1 << ALT_KEY_IS_DOWN_SHIFT)) != 0);

            // If the key was down before, but now isnt, that means that the key got released, and the
            // other way around This if statment pervents us from being flooded by key-repeat messages
            // (meaning that a key is being held down)
            if (WasDown != IsDown)
            {

                if (VKCode == VK_ESCAPE)
                {
                    AppState.Stop();
                }
                else if (VKCode == VK_F4 && AltKeyWasDown)
                {
                    AppState.Stop();
                }
            }
        }
        break;

        default: {
            TranslateMessage(&Message);
            DispatchMessageA(&Message);
        }
        break;
        }
    }
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow)
{

    // Creating our window class
    WNDCLASSEXA WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEXA);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32QuixotismEngineWindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "QuixotismEngineWindowClass";

    // Registering window class
    if (RegisterClassExA(&WindowClass))
    {
        win32_app_state AppState(InitWindowClientWidth, InitWindowClientHeight);
        const auto [ClientWidth, ClientHeight] = AppState.GetClientDimensions();
        RECT InitWindowSize;
        InitWindowSize.left = InitWindowPosX;
        InitWindowSize.right = InitWindowPosX + ClientWidth;
        InitWindowSize.top = InitWindowPosY;
        InitWindowSize.bottom = InitWindowPosY + ClientHeight;
        AdjustWindowRectEx(&InitWindowSize, WS_OVERLAPPEDWINDOW | WS_VISIBLE, false, 0);

        // Create window
        HWND Window =
            CreateWindowExA(0, WindowClass.lpszClassName, "QuixotismEngine", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            InitWindowSize.left, InitWindowSize.top, (InitWindowSize.right - InitWindowSize.left),
                            (InitWindowSize.bottom - InitWindowSize.top), nullptr, nullptr, Instance, &AppState);

        // check if we got a valid window handle and state
        if (Window)
        {
            AppState.Start();
            while (AppState.IsRunning())
            {
                Win32ProcessWindowMessages(AppState);
            }
        }
    }
    return 0;
}