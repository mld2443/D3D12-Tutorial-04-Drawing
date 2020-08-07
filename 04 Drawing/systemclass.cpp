////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "systemclass.h"


//////////////////////
// GLOBAL VARIABLES //
//////////////////////

// Global pointer to our SystemClass instantiation.  Used to redirect windows messages to SystemClass.
static SystemClass *g_ApplicationHandle = nullptr;


// Static function for C-style callback that Windows needs.
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        // Check if the window is being destroyed or closed.
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

        // All other messages pass to the message handler in the system class.
    default:
        if (g_ApplicationHandle)
        {
            return g_ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
        }
        else
        {
            return DefWindowProc(hwnd, umsg, wparam, lparam);
        }
    }
}


SystemClass::SystemClass()
    : m_Input(std::make_unique<InputClass>())
{
    // Initialize the windows API.
    InitializeWindows();

    // Create the engine object.  This object will handle rendering all the graphics for this application.
    m_Engine = std::make_unique<EngineClass>(m_hwnd, m_xResolution, m_yResolution, m_fullscreen);
}


SystemClass::~SystemClass()
{
    // Shutdown the window.
    ShutdownWindows();
}


void SystemClass::Run()
{
    // Initialize the message structure.
    MSG msg{};

    // Loop until there is a quit message from the window or the user.
    bool running = true;
    while (running)
    {
        // Handle the Windows messages.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // If windows signals to end the application then exit out.
        if (msg.message == WM_QUIT)
        {
            running = false;
        }
        else
        {
            // Otherwise do the frame processing.
            running = Frame();
        }
    }
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
    // Check if a key has been pressed on the keyboard.
    case WM_KEYDOWN:
        // If a key is pressed send it to the input object so it can record that state.
        m_Input->KeyDown(static_cast<UINT>(wparam));
        return 0;

    // Check if a key has been released on the keyboard.
    case WM_KEYUP:
        // If a key is released then send it to the input object so it can unset the state for that key.
        m_Input->KeyUp(static_cast<UINT>(wparam));
        return 0;

    // Any other messages send to the default message handler as our application won't make use of them.
    default:
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
}


bool SystemClass::Frame()
{
    // Check if the user pressed escape and wants to exit the application.
    if (m_Input->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    // Do the frame processing for the graphics object.
    m_Engine->Frame();

    return true;
}


void SystemClass::InitializeWindows()
{


    // Get an external pointer to this object.
    g_ApplicationHandle = this;

    // Get the instance of this application.
    m_hinstance = GetModuleHandle(NULL);

    // Setup the windows class with default settings.
    WNDCLASSEX wc{};
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = m_hinstance;
    wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm       = wc.hIcon;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = m_applicationName;
    wc.cbSize        = sizeof(WNDCLASSEX);

    // Register the window class.
    THROW_IF_FALSE(
        RegisterClassEx(&wc),
        "Unable to register the window class."
    );

    // Set the position of the window to the top left corner.
    int posX = 0, posY = 0;

    // Determine the resolution of the clients desktop screen.
    UINT screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    UINT screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
    if (m_fullscreen)
    {
        // If we're in fullscreen, update the resolution with that of the display.
        m_xResolution = screenWidth;
        m_yResolution = screenHeight;

        // If full screen set the screen to maximum size of the users desktop and 32bit.
        DEVMODE dmScreenSettings{};
        dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth  = m_xResolution;
        dmScreenSettings.dmPelsHeight = m_yResolution;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
    }
    else
    {
        // Place the window in the middle of the screen.
        posX = (screenWidth  - m_xResolution) / 2;
        posY = (screenHeight - m_yResolution) / 2;
    }

    // Create the window with the screen settings and get the handle to it.
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
                            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
                            posX, posY, m_xResolution, m_yResolution, NULL, NULL, m_hinstance, NULL);
    THROW_IF_FALSE(
        m_hwnd,
        "Unable to create the window."
    );

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    // Hide the mouse cursor.
    ShowCursor(false);
}


void SystemClass::ShutdownWindows()
{
    // Show the mouse cursor.
    ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    if (m_fullscreen)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    // Remove the window.
    DestroyWindow(m_hwnd);
    m_hwnd = NULL;

    // Remove the application instance.
    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = NULL;

    // Release the pointer to this class.
    g_ApplicationHandle = nullptr;
}
