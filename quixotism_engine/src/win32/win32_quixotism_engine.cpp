#include "win32_quixotism_engine.hpp"
#include "GL/glew.h"
#include "GL/wglew.h"
#include "debug_out.hpp"

#include <Windows.h>
#include <Windowsx.h>

#include "quixotism_engine.hpp"
#include "quixotism_input.hpp"
#include "quixotism_platform_services.hpp"
#include "win32_app_state.hpp"
#include "win32_io.hpp"
#include "win32_timer.hpp"

#include <array>
#include <memory>
#include <string>

INTERNAL auto *Win32GetAppState(HWND Window)
{
    LONG_PTR Ptr = GetWindowLongPtr(Window, GWLP_USERDATA);
    auto *WindowState = reinterpret_cast<win32_app_state *>(Ptr);
    return WindowState;
}

INTERNAL bool32 Win32OpenDebugConsole()
{
    bool32 Result = AllocConsole();
    if (Result == TRUE)
    {
        freopen_s(reinterpret_cast<FILE **>(stdout), "CONOUT$", "w", stdout);
        DEBUG_OUT("-----> Quixotism DEBUG console! <-----");
    }
    return Result;
}

INTERNAL void Win32ProcessMouseMovement(HWND Window, win32_app_state &AppState, engine_input *Input)
{
    POINT MousePos = {};
    if (AppState.IsCursorTrapped() == TRUE)
    {
        GetCursorPos(&MousePos);
    }

    const auto [ClientWidth, ClientHeight] = AppState.GetClientDimensions();
    int32 CenterX = ClientWidth / 2;
    int32 CenterY = ClientHeight / 2;
    auto XDiff = static_cast<real32>(MousePos.x - CenterX);
    auto YDiff = static_cast<real32>(MousePos.y - CenterY);

    Input->SetCursorPositionChange(XDiff, YDiff);
    POINT CenterPos = {CenterX, CenterY};
    ClientToScreen(Window, &CenterPos);

    if (AppState.IsCursorTrapped() == TRUE)
    {
        SetCursorPos(CenterPos.x, CenterPos.y);
    }
}

INTERNAL std::pair<int32, int32> Win32GetClientDimension(HWND WindowHandle)
{
    RECT ClientRect;
    GetClientRect(WindowHandle, &ClientRect);
    return std::make_pair(ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top);
}

INTERNAL void Win32ProcessKeyboardInput(button_state &NewState, bool32 IsDown)
{
    if (NewState.EndedDown != IsDown)
    {
        NewState.EndedDown = IsDown;
        NewState.HalfTransitionCount++;
    }
}

INTERNAL bool32 Win32SetPixelFormat(HWND Window)
{
    PIXELFORMATDESCRIPTOR PixelFormatDesc = {};
    PixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    PixelFormatDesc.nVersion = 1;
    PixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    PixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
    PixelFormatDesc.cColorBits = PIXELFORMAT_COLOR_BITS;
    PixelFormatDesc.cAlphaBits = PIXELFORMAT_ALPHA_BITS;
    PixelFormatDesc.cDepthBits = PIXELFORMAT_DEPTH_BITS;
    PixelFormatDesc.cStencilBits = PIXELFORMAT_STENCIL_BITS;
    PixelFormatDesc.iLayerType = PFD_MAIN_PLANE;
    HDC WindowDC = GetDC(Window);
    int32 SuggestedPixelFormat = ChoosePixelFormat(WindowDC, &PixelFormatDesc);
    // Check if we got a valid suggestion of pixel format
    if (SuggestedPixelFormat != 0)
    {
        // continue only if we succeed in setting the pxel format
        if (SetPixelFormat(WindowDC, SuggestedPixelFormat, &PixelFormatDesc) != FALSE)
        {
            return TRUE;
        }
    }
    ReleaseDC(Window, WindowDC);
    return FALSE;
}

INTERNAL auto Win32InitializeOpenGL(HWND Window)
{
    bool32 InitializationResult = FALSE;

    // continue only if we succeed in setting the pxel format
    if (Win32SetPixelFormat(Window) != FALSE)
    {
        HDC WindowDC = GetDC(Window);
        // Creat opengl context
        HGLRC OpenGLRenderingContext = wglCreateContext(WindowDC);

        // Check if we got valid rendering context
        if (OpenGLRenderingContext)
        {
            if (wglMakeCurrent(WindowDC, OpenGLRenderingContext) != 0)
            {
                // initialize glew, if we fail, we should stop initialization of opengl, as we need glew right now.
                if (glewInit() == GLEW_OK)
                {
                    const uint32 WGL_CONTEXT_ATTRIBS_COUNT = 9;
                    // Context attribs, we specify the version we want to use...
                    std::array<int32, WGL_CONTEXT_ATTRIBS_COUNT> WGL_ContextAttribs = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                                                                                       OPENGL_DESIRED_MAJOR_VERION,
                                                                                       WGL_CONTEXT_MINOR_VERSION_ARB,
                                                                                       OPENGL_DESIRED_MINOR_VERION,
                                                                                       WGL_CONTEXT_FLAGS_ARB,
                                                                                       0,
                                                                                       WGL_CONTEXT_PROFILE_MASK_ARB,
                                                                                       WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                                                                       0};

                    // Check for "WGL_ARB_create_context" as we may want to specify our own version of opengl
                    if (wglewIsSupported("WGL_ARB_create_context") == GL_TRUE)
                    {
                        // Create context with specified attributes
                        HGLRC OpenGLRenderingContextARB =
                            wglCreateContextAttribsARB(WindowDC, nullptr, WGL_ContextAttribs.data());
                        if (OpenGLRenderingContextARB)
                        {
                            wglMakeCurrent(nullptr, nullptr);
                            wglDeleteContext(OpenGLRenderingContext);
                            wglMakeCurrent(WindowDC, OpenGLRenderingContextARB);
                        }
                    }
                    else
                    {
                        DEBUG_OUT("----------> wglCreateContextAtribs not specified (WARNING)");
                    }
                    // We scucceded in initializing opengl in our window
                    // Print out opengl version...
                    const auto *GLVersion = glGetString(GL_VERSION);
                    std::string OGLText("OpenGL VERSION: ");
                    std::string GLVersionStr(reinterpret_cast<const char *>(GLVersion));
                    DEBUG_OUT((OGLText + GLVersionStr + "\n").c_str());

                    // NOTE: IMPORTANT: Activates vsync (swap buffers on vertical blank)!]
                    wglSwapIntervalEXT(SwapIntervalCount);

                    InitializationResult = TRUE;
                }
                else
                {
                    // deinitialize the current rendering context
                    wglMakeCurrent(nullptr, nullptr);
                    wglDeleteContext(OpenGLRenderingContext);
                    DEBUG_OUT("GLEW could not be initialized... (ABORTING)");
                }
            }
        }
        ReleaseDC(Window, WindowDC);
    }
    return InitializationResult;
}

// Main window callback procedure
LRESULT CALLBACK Win32QuixotismEngineWindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = {};

    switch (Message)
    {

    case WM_CREATE: {
        auto *Create = reinterpret_cast<CREATESTRUCT *>(LParam);
        auto *AppState = reinterpret_cast<win32_app_state *>(Create->lpCreateParams);
        SetWindowLongPtr(Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(AppState));
        if (Win32InitializeOpenGL(Window) == FALSE)
        {
            // Could not initialize opengl..
            // NOTE: Currently there is only opengl, if we fail to initialize it, the application has to close. If we
            // were to support other (like Direct3D), then we could try to fallback to those.
            PostQuitMessage(-1);
        }
        // Initialize the cursor to the center of the window...
        auto const [Width, Height] = AppState->GetClientDimensions();
        SetCursorPos(Width / 2, Height / 2);
    }
    break;

    case WM_SIZE: {
        auto const [Width, Height] = Win32GetClientDimension(Window);
        auto *AppState = Win32GetAppState(Window);
        AppState->SetClientDimensions(Width, Height);
        if (AppState->IsCursorTrapped())
        {
            RECT PreviousCursorClip = AppState->GetPreviousCursorClip();
            RECT WindowCursorClip;
            GetWindowRect(Window, &WindowCursorClip);
            ClipCursor(&WindowCursorClip);
            AppState->EnableCursorTrap(WindowCursorClip, PreviousCursorClip);
        }
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
        auto *AppState = Win32GetAppState(Window);
        if (WParam == TRUE)
        {
            // Window is being activated
            RECT PreviousCursorClip;
            GetClipCursor(&PreviousCursorClip);
            RECT WindowCursorClip;
            GetWindowRect(Window, &WindowCursorClip);
            ClipCursor(&WindowCursorClip);
            AppState->EnableCursorTrap(WindowCursorClip, PreviousCursorClip);
        }
        else
        {
            // Window is being deactivated
            auto PreviousCursorClip = AppState->GetPreviousCursorClip();
            ClipCursor(&PreviousCursorClip);
            AppState->DisableCursorTrap();
        }
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

void Win32ProcessWindowMessages(HWND Window, win32_app_state &AppState, engine_input *Input)
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

            auto &KeyboardController = Input->GetKeybaordController();

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
                if (VKCode == 'C')
                {
                    if (IsDown)
                    {
                        DEBUG_OUT("TOGGLING MOUSE VISIBILITY");
                        Win32ProcessKeyboardInput(KeyboardController.Start, IsDown);
                        AppState.ToggleCursorHidden();
                        if (AppState.IsCursorHidden())
                        {
                            ShowCursor(FALSE);
                        }
                        else
                        {
                            ShowCursor(TRUE);
                        }
                    }
                }
                else if (VKCode == VK_ESCAPE)
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

#ifdef BUILD_INTERNAL
    auto DebugConsoleOK = Win32OpenDebugConsole();
    if (DebugConsoleOK == TRUE)
    {
        DEBUG_OUT("DEBUG BUILD");
    }
    else
    {
        return -1;
    }
#endif

    const real32 TargetSecondsPerFrame = 0.1667F;

    // Creating our window class
    WNDCLASSEXA WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEXA);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32QuixotismEngineWindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursorA(nullptr, IDC_ARROW); // NOLINT
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
        HWND Window = CreateWindowExA(
            0, WindowClass.lpszClassName, "QuixotismEngine", (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_THICKFRAME,
            InitWindowSize.left, InitWindowSize.top, (InitWindowSize.right - InitWindowSize.left),
            (InitWindowSize.bottom - InitWindowSize.top), nullptr, nullptr, Instance, &AppState);

        // check if we got a valid window handle and state
        if (Window)
        {
            engine_input Input[2];
            engine_input *NewInput = &Input[0];
            engine_input *PrevInput = &Input[1];

            NewInput->SetTimeStep(TargetSecondsPerFrame);

            const auto [Width, Height] = AppState.GetClientDimensions();
            quixotism_window_info WindowInfo = {Width, Height};

            platform_services PlatformServices = {};
            PlatformServices.ReadFile = Win32ReadFile;
            PlatformServices.GetTime = GetRunningTime;

            QuixotismEngine = std::make_unique<quixotism_engine>(WindowInfo, PlatformServices);
            QuixotismEngine->Init();

            AppState.Start();

            auto LastTimestamp = win32_timer::GetTimestamp();
            while (AppState.IsRunning())
            {
                Win32ProcessWindowMessages(Window, AppState, NewInput);
                Win32ProcessMouseMovement(Window, AppState, NewInput);

                QuixotismEngine->UpdateAndRender();

                HDC WindowDC = GetDC(Window);
                SwapBuffers(WindowDC);
                ReleaseDC(Window, WindowDC);

                SwapPointers(NewInput, PrevInput);

                auto EndTimestamp = win32_timer::GetTimestamp();
                auto MillisecondsPerFrame = win32_timer::GetTimeDifference<milliseconds>(LastTimestamp, EndTimestamp);
                real64 FPS = MILLISECONDS_IN_SECONDS / MillisecondsPerFrame.Count;

                const uint32 BufferSize = 256;
                char Buffer[BufferSize];
                sprintf_s(Buffer, "Milliseconds/frame: %.02fms --FPS: %.02f\n", MillisecondsPerFrame.Count, FPS);
                OutputDebugStringA(&Buffer[0]);

                LastTimestamp = win32_timer::GetTimestamp();
            }
        }
    }
    return 0;
}