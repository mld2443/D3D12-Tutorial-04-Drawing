////////////////////////////////////////////////////////////////////////////////
// Filename: engineclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "engineclass.h"


EngineClass::EngineClass(HWND hWnd, UINT xResolution, UINT yResolution, bool fullscreen)
    : D3DClass(hWnd, xResolution, yResolution, fullscreen, m_vsyncEnabled)
    , m_Camera(std::make_unique<CameraClass>(xResolution, yResolution, 45.0f))
    , m_Pipeline(std::make_unique<PipelineClass>(GetDevice(), GetBufferIndex()))
    , m_Context(std::make_unique<InstanceContextClass>(GetDevice(),
                                                       GetBufferIndex(),
                                                       m_Camera->GetViewMatrix(),
                                                       m_Camera->GetProjectionMatrix(),
                                                       xResolution, yResolution))
    , m_Geometry(std::make_unique<QuadClass>(GetDevice()))
{
    // Move the camera back so we can see our scene.
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

    // Set the backdground to a neutral gray color.
    SetClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}


EngineClass::~EngineClass()
{
    // Wait for all frames to finish before the unique_ptrs can release.
    WaitForAllFrames();
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
    SubmitToQueue(lists, m_vsyncEnabled);
}


void EngineClass::Render()
{
    // Advance the buffer index and wait for the corresponding buffer to be available.
    WaitForNextAvailableFrame();

    // Open our pipeline, set a transition barrier, then reset the RTV and DSV.
    m_Pipeline->Open();
    m_Pipeline->SetState(m_Context->GetState());
    m_Pipeline->AddBarrier(StartBarrier());
    ResetViews(m_Pipeline->GetCommandList());

    // Communicate the matrices to the vertex shader and submit the geometry to the pipeline.
    m_Context->SetShaderParameters(m_Pipeline->GetCommandList());
    m_Geometry->Render(m_Pipeline->GetCommandList());

    // Add a final transition barrier and close the pipeline.
    m_Pipeline->AddBarrier(FinishBarrier());
    m_Pipeline->Close();
}
