////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Triangle = nullptr;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenHeight, int screenWidth, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenHeight, screenWidth, hwnd, VSYNC_ENABLED, FULL_SCREEN);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial parameters of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->SetLookDirection(0.0f, 0.0f, 1.0f);

	// Create the triangle object.
	m_Triangle = new TriangleClass;
	if (!m_Triangle)
	{
		return false;
	}

	// Initialize the triangle object.
	result = m_Triangle->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize geometry object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the geometry object.
	if (m_Triangle)
	{
		m_Triangle->Shutdown();
		delete m_Triangle;
		m_Triangle = nullptr;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;


	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{
	bool result;


	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Submit the geometry buffers to the render pipeline.
	m_Triangle->Render();

	// Use the Direct3D object to render the scene.
	result = m_Direct3D->Render(0.2f, 0.2f, 0.2f, 1.0f);
	if (!result)
	{
		return false;
	}

	return true;
}
