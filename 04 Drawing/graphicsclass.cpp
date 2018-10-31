////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "graphicsclass.h"


GraphicsClass::GraphicsClass(HWND hwnd, UINT screenWidth, UINT screenHeight)
{
	// Create the camera object with a field of view of 45 degrees.
	m_Camera = new CameraClass(screenWidth, screenHeight, 45.0f);

	// Move the camera back so we can see our scene.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass(hwnd, screenWidth, screenHeight, VSYNC_ENABLED, FULL_SCREEN);

	// Create and initialize the pipeline object.
	m_Pipeline = new SoloPipelineClass(m_Direct3D->GetDevice(), m_Direct3D->GetBufferIndex(), screenWidth, screenHeight, m_Camera->GetScreenNear(), m_Camera->GetScreenFar());

	// Create and initialize the triangle object.
	m_Geometry = new TriangleClass(m_Direct3D->GetDevice());
}


GraphicsClass::~GraphicsClass()
{
	// Wait for all frames to finish before releasing anything.
	m_Direct3D->WaitForAllFrames();

	// Release the objects.
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Geometry);
	SAFE_DELETE(m_Pipeline);
	SAFE_DELETE(m_Direct3D);
}


void GraphicsClass::Frame()
{
	// Render the graphics scene.
	Render();
}


void GraphicsClass::Render()
{
	XMMATRIX view, projection;
	std::vector<ID3D12CommandList*> lists;


	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the camera's view and projection matrices.
	view = m_Camera->GetViewMatrix();
	projection = m_Camera->GetProjectionMatrix();

	// Advance the buffer index and wait for the corresponding buffer to be available.
	m_Direct3D->WaitForNextAvailableFrame();

	// Start our pipeline, preparing it for drawing commands.
	m_Pipeline->OpenPipeline(m_Direct3D->GetBufferIndex());

	// Put the first transition barrier on the command list and clear the RTV and DSV.
	m_Direct3D->BeginScene(m_Pipeline->GetCommandList(), 0.2f, 0.2f, 0.2f, 1.0f);

	// Communicate the world, view, and projection matrices to the vertex shader.
	m_Pipeline->SetPipelineParameters(m_Direct3D->GetBufferIndex(), view, projection);

	// Submit the geometry buffers to the render pipeline.
	m_Geometry->Render(m_Pipeline->GetCommandList());

	// Add a final transition barrier to the queue.
	m_Direct3D->EndScene(m_Pipeline->GetCommandList());

	// Close our pipeline so its command list can be queued.
	m_Pipeline->ClosePipeline();

	// Collect all one of our command lists to be drawn this frame.
	lists.push_back(m_Pipeline->GetCommandList());

	// Finish the scene and submit our lists for drawing.
	m_Direct3D->SubmitToQueue(lists);
}
