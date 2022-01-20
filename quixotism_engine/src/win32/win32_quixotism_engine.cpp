#include "win32_quixotism_engine.hpp"
#include "GL/glew.h"
#include "GL/wglew.h"
#include "debug_out.hpp"

#include <Windows.h>

#include "quixotism_engine.hpp"
#include "quixotism_input.hpp"
#include "win32_app_state.hpp"
#include "win32_timer.hpp"

#include <array>
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
        DEBUG_OUT("-----> Tiny Renderer DEBUG console! <-----");
    }
    return Result;
}

bool32 Win32SetPixelFormat(HWND Window)
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

    win32_timer Timer;

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

            auto LastTimestamp = win32_timer::GetTimestamp();
            while (AppState.IsRunning())
            {
                Win32ProcessWindowMessages(AppState);

                HDC WindowDC = GetDC(Window);
                SwapBuffers(WindowDC);
                ReleaseDC(Window, WindowDC);

                auto EndTimestamp = win32_timer::GetTimestamp();
                auto MillisecondsPerFrame = Timer.GetTimeDifference<milliseconds>(LastTimestamp, EndTimestamp);
                real64 FPS = MILLISECONDS_IN_SECONDS / MillisecondsPerFrame.Count;

                char Buffer[256];
                sprintf_s(Buffer, "Milliseconds/frame: %.02fms --FPS: %.02f\n", MillisecondsPerFrame.Count, FPS);
                OutputDebugStringA(Buffer);

                LastTimestamp = win32_timer::GetTimestamp();
            }
        }
    }
    return 0;
}