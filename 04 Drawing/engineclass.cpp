////////////////////////////////////////////////////////////////////////////////
// Filename: engineclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "engineclass.h"


EngineClass::EngineClass(HWND hwnd, UINT xResolution, UINT yResolution, bool fullscreen)
    : m_Camera(std::make_unique<CameraClass>(xResolution, yResolution, 45.0f))
    , m_Direct3D(std::make_unique<D3DClass>(hwnd, xResolution, yResolution, fullscreen, m_vsyncEnabled))
    , m_Pipeline(std::make_unique<PipelineClass>(m_Direct3D->GetDevice(), m_Direct3D->GetBufferIndex()))
    , m_Context(std::make_unique<InstanceContextClass>(m_Direct3D->GetDevice(),
                                                       m_Direct3D->GetBufferIndex(),
                                                       m_Camera->GetViewMatrix(),
                                                       m_Camera->GetProjectionMatrix(),
                                                       xResolution, yResolution))
    , m_Geometry(std::make_unique<QuadClass>(m_Direct3D->GetDevice()))
{
    // Move the camera back so we can see our scene.
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

    // Set the backdground to a neutral gray color.
    m_Direct3D->SetClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}


EngineClass::~EngineClass()
{
    // Wait for all frames to finish before the unique_ptrs can release.
    m_Direct3D->WaitForAllFrames();
}


void EngineClass::Frame()
{
    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Render the graphics scene.
    Render();

    // Collect all one of our command lists to be drawn this frame, (only one for now).
    std::vector<ID3D12CommandList*> lists;
    lists.push_back(m_Pipeline->GetCommandList());

    // Finish the scene and submit our lists for drawing.
    m_Direct3D->SubmitToQueue(lists, m_vsyncEnabled);
}


void EngineClass::Render()
{
    // Advance the buffer index and wait for the corresponding buffer to be available.
    m_Direct3D->WaitForNextAvailableFrame();

    // Open our pipeline, set a transition barrier, then reset the RTV and DSV.
    m_Pipeline->Open();
    m_Pipeline->SetState(m_Context->GetState());
    m_Pipeline->AddBarrier(m_Direct3D->StartBarrier());
    m_Direct3D->ResetViewsCallback(m_Pipeline->GetCommandList());

    // Communicate the matrices to the vertex shader and submit the geometry to the pipeline.
    m_Context->SetShaderParameters(m_Pipeline->GetCommandList());
    m_Geometry->Render(m_Pipeline->GetCommandList());

    // Add a final transition barrier and close the pipeline.
    m_Pipeline->AddBarrier(m_Direct3D->FinishBarrier());
    m_Pipeline->Close();
}
