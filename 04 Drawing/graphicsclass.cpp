////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
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
	float aspectRatio;


	// Calculate the ratio of the screen for the camera.
	aspectRatio = (float)screenWidth / (float)screenHeight;

	// Create the camera object.
	m_Camera = new CameraClass(screenWidth, screenHeight, PI / 4.0f);

	// Set the initial parameters of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass(hwnd, screenHeight, screenWidth, VSYNC_ENABLED, FULL_SCREEN);

	// Create the pipeline object.
	m_Pipeline = new SoloPipelineClass;

	// Initialize the pipeline object.
	result = m_Pipeline->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetBufferIndex(), screenWidth, screenHeight, m_Camera->GetScreenNear(), m_Camera->GetScreenFar());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize pipeline object.", L"Error", MB_OK);
		return false;
	}

	// Create the triangle object.
	m_Geometry = new TriangleClass(m_Direct3D->GetDevice());

	return true;
}


void GraphicsClass::Shutdown()
{
	// Wait for all frames to finish before releasing anything.
	m_Direct3D->WaitOnAllFrames();

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// Release the geometry object.
	if (m_Geometry)
	{
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
	XMMATRIX view, projection;
	std::vector<ID3D12CommandList*> lists;


	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the camera's view matrix.
	view = m_Camera->GetViewMatrix();
	projection = m_Camera->GetProjectionMatrix();

	// Wait on the frame last in this buffer index.
	m_Direct3D->WaitForPreviousFrame();

	// Start our pipeline, preparing it for drawing commands.
	result = m_Pipeline->OpenPipeline(m_Direct3D->GetBufferIndex());
	if (!result)
	{
		return false;
	}

	// Put the first transition barrier on the command list and clear the RTV and DSV.
	m_Direct3D->BeginScene(m_Pipeline->GetCommandList(), 0.2f, 0.2f, 0.2f, 1.0f);

	// Start our pipeline, preparing it for drawing commands.
	result = m_Pipeline->SetPipelineParameters(m_Direct3D->GetBufferIndex(), view, projection);
	if (!result)
	{
		return false;
	}

	// Submit the geometry buffers to the render pipeline.
	m_Geometry->Render(m_Pipeline->GetCommandList());

	// Add a final transition barrier to the queue.
	m_Direct3D->EndScene(m_Pipeline->GetCommandList());

	// Close our pipeline so its command list can be queued.
	result = m_Pipeline->ClosePipeline();
	if (!result)
	{
		return false;
	}

	// Collect all one of our command lists to be drawn this frame.
	lists.push_back(m_Pipeline->GetCommandList());

	// Finish the scene and submit our lists for drawing.
	m_Direct3D->SubmitToQueue(lists);

	return true;
}
