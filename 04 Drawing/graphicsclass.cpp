////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Geometry = nullptr;
	m_Pipeline = nullptr;
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

	// Create the pipeline object.
	m_Pipeline = new SoloPipelineClass;
	if (!m_Pipeline)
	{
		return false;
	}

	// Initialize the pipeline object.
	result = m_Pipeline->Initialize(m_Direct3D->GetDevice(), hwnd, m_Direct3D->GetBufferIndex(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize pipeline object.", L"Error", MB_OK);
		return false;
	}

	// Create the triangle object.
	m_Geometry = new TriangleClass;
	if (!m_Geometry)
	{
		return false;
	}

	// Initialize the triangle object.
	result = m_Geometry->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize geometry object.", L"Error", MB_OK);
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

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// Release the geometry object.
	if (m_Geometry)
	{
		m_Geometry->Shutdown();
		delete m_Geometry;
		m_Geometry = nullptr;
	}

	// Release the pipeline object.
	if (m_Pipeline)
	{
		m_Pipeline->Shutdown();
		delete m_Pipeline;
		m_Pipeline = nullptr;
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
	XMMATRIX view;
	std::vector<ID3D12CommandList*> lists;


	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the camera's view matrix.
	m_Camera->GetViewMatrix(view);

	// Start our pipeline, preparing it for drawing commands.
	result = m_Pipeline->ResetCommandList(m_Direct3D->GetBufferIndex());
	if (!result)
	{
		return false;
	}

	// Put the first transition barrier on the command list and clear the RTV and DSV.
	m_Direct3D->BeginScene(m_Pipeline->GetCommandList(), 0.2f, 0.2f, 0.2f, 1.0f);

	// Start our pipeline, preparing it for drawing commands.
	result = m_Pipeline->BeginPipeline(m_Direct3D->GetBufferIndex(), view);
	if (!result)
	{
		return false;
	}

	// Submit the geometry buffers to the render pipeline.
	m_Geometry->Render(m_Pipeline->GetCommandList());

	// Add a final transition barrier to the queue.
	m_Direct3D->EndScene(m_Pipeline->GetCommandList());

	// Close our pipeline, so its command list can be queued.
	result = m_Pipeline->EndPipeline();
	if (!result)
	{
		return false;
	}

	// Collect all one of our command lists to be drawn this frame.
	lists.push_back(m_Pipeline->GetCommandList());

	// Finish the scene and submit our lists for drawing.
	result = m_Direct3D->SubmitToQueue(lists);

	return true;
}
