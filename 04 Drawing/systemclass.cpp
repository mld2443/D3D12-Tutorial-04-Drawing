////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "systemclass.h"


SystemClass::SystemClass()
{
	// Initialize the windows API.
	InitializeWindows();

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new EngineClass(m_hwnd, m_xResolution, m_yResolution, m_fullscreen);
}


SystemClass::~SystemClass()
{
	// Release the graphics and input objects.
	SAFE_DELETE(m_Graphics);
	SAFE_DELETE(m_Input);

	// Shutdown the window.
	ShutdownWindows();
}


void SystemClass::Run()
{
	MSG msg;
	bool running;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(msg));

	// Loop until there is a quit message from the window or the user.
	running = true;
	while (running)
	{
		// Handle the windows messages.
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

	return;
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
	m_Graphics->Frame();

	return true;
}


void SystemClass::InitializeWindows()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	UINT screenWidth, screenHeight;
	int posX, posY;


	// Get an external pointer to this object.
	g_ApplicationHandle = static_cast<void*>(this);

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Setup the windows class with default settings.
	wc.style =			CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc =	WndProc;
	wc.cbClsExtra =		0;
	wc.cbWndExtra =		0;
	wc.hInstance =		m_hinstance;
	wc.hIcon =			LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm =		wc.hIcon;
	wc.hCursor =		LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground =	static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName =	NULL;
	wc.lpszClassName =	m_applicationName;
	wc.cbSize =			sizeof(WNDCLASSEX);

	// Register the window class.
	THROW_IF_FALSE(
		RegisterClassEx(&wc),
		L"Unable to register the window class.",
		L"Window Initialization Failure"
	);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (m_fullscreen)
	{
		// If we're in fullscreen, update the resolution with that of the display.
		m_xResolution = screenWidth;
		m_yResolution = screenHeight;

		// If full screen set the screen to maximum size of the users desktop and 32bit.
		ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize =		sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth =	m_xResolution;
		dmScreenSettings.dmPelsHeight =	m_yResolution;
		dmScreenSettings.dmBitsPerPel =	32;
		dmScreenSettings.dmFields =		DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// Place the window in the middle of the screen.
		posX = (screenWidth - m_xResolution) / 2;
		posY = (screenHeight - m_yResolution) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
							WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
							posX, posY, m_xResolution, m_yResolution, NULL, NULL, m_hinstance, NULL);
	THROW_IF_FALSE(
		m_hwnd,
		L"Unable to create the window.",
		L"Window Creation Failure"
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


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	// Check if the window is being destroyed or closed.
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	// All other messages pass to the message handler in the system class.
	default:
		return static_cast<SystemClass*>(g_ApplicationHandle)->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}
