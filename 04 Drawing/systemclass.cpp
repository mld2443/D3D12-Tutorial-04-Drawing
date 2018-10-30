////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "systemclass.h"


SystemClass::SystemClass() :
	m_applicationName(L"04 Drawing")
{
	UINT windowHeight, windowWidth;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	windowHeight = 0;
	windowWidth = 0;

	// Initialize the windows API.
	InitializeWindows(windowHeight, windowWidth);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass(m_hwnd, windowWidth, windowHeight);
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
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(msg));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
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
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
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


bool SystemClass::InitializeWindows(UINT& screenHeight, UINT& screenWidth)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
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
	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	// Determine the resolution of the clients desktop screen.
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	screenWidth = GetSystemMetrics(SM_CXSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize =		sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsHeight =	screenHeight;
		dmScreenSettings.dmPelsWidth =	screenWidth;
		dmScreenSettings.dmBitsPerPel =	32;
		dmScreenSettings.dmFields =		DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
							WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
							posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	if (!m_hwnd)
	{
		return false;
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return true;
}


void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
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
