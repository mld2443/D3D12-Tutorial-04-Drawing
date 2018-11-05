////////////////////////////////////////////////////////////////////////////////
// Filename: engineclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "engineclass.h"


EngineClass::EngineClass(HWND hwnd, UINT xResolution, UINT yResolution, bool fullscreen) :
	m_Camera(make_unique<CameraClass>(xResolution, yResolution, 45.0f)),
	m_Direct3D(make_unique<D3DClass>(hwnd, xResolution, yResolution, fullscreen, m_vsyncEnabled)),
	m_InstanceContext(make_unique<InstanceContextClass>(m_Direct3D->GetDevice(), xResolution, yResolution, m_Camera->GetScreenNear(), m_Camera->GetScreenFar())),
	m_Quads(make_unique<QuadClass>(m_Direct3D->GetDevice()))
	//m_ColorContext(make_unique<ColorContextClass>(m_Direct3D->GetDevice(), xResolution, yResolution, m_Camera->GetScreenNear(), m_Camera->GetScreenFar())),
	//m_Triangle(make_unique<TriangleClass>(m_Direct3D->GetDevice()))
{
	// Move the camera back so we can see our scene.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
}


EngineClass::~EngineClass()
{
	// Wait for all frames to finish before the unique_ptrs can release.
	m_Direct3D->ShutdownAllFrames();
}


void EngineClass::Frame()
{
	std::vector<ID3D12CommandList*> lists;


	// Render the graphics scene.
	Render();

	// Collect all one of our command lists to be drawn this frame.
	lists.push_back(m_InstanceContext->GetCommandList());

	// Finish the scene and submit our lists for drawing.
	m_Direct3D->SubmitToQueue(lists, m_vsyncEnabled);
}


void EngineClass::Render()
{
	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Advance the buffer index and wait for the corresponding buffer to be available.
	m_Direct3D->WaitForNextAvailableFrame();

	// Start our pipeline, set a transition barrier, then clear the RTV and DSV.
	m_InstanceContext->OpenPipeline(m_Direct3D->GetBufferIndex());
	m_Direct3D->BeginScene(m_InstanceContext->GetCommandList(), 0.2f, 0.2f, 0.2f, 1.0f);

	// Communicate the world, view, and projection matrices to the vertex shader.
	m_InstanceContext->SetPipelineParameters(
		m_Direct3D->GetBufferIndex(),
		m_Camera->GetViewMatrix(),
		m_Camera->GetProjectionMatrix()
	);

	// Submit the geometry buffers to the render pipeline.
	m_Quads->Render(m_InstanceContext->GetCommandList());

	// Add a final transition barrier and close the pipeline.
	m_Direct3D->EndScene(m_InstanceContext->GetCommandList());
	m_InstanceContext->ClosePipeline();
}
