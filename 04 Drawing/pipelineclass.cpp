////////////////////////////////////////////////////////////////////////////////
// Filename: pipelineclass.h
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "pipelineclass.h"


PipelineClass::PipelineClass(ID3D12Device *device, const UINT &frameIndex, D3D12_COMMAND_LIST_TYPE type)
    : r_frameIndex(frameIndex)
{
    // Create command allocators, one for each frame.
    for (uint32_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
    {
        THROW_IF_FAILED(
            device->CreateCommandAllocator(
                type,
                IID_PPV_ARGS(m_commandAllocators[i].ReleaseAndGetAddressOf())),
            "Unable to create the command allocator object."
        );
    }

    // Create a command list.  We use the first command allocator, since allocators get reset before use.
    THROW_IF_FAILED(
        device->CreateCommandList(
            0,
            type,
            m_commandAllocators[0].Get(),
            nullptr,
            IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())),
        "Unable to create the command list object."
    );

    // Initially we need to close the command list during initialization as it is created in a recording state.
    Close();

    NameD3DResources();
}


ID3D12GraphicsCommandList * PipelineClass::GetCommandList()
{
    return m_commandList.Get();
}


void PipelineClass::Open()
{
    // Reset the memory that was holding the previously submitted command list.
    THROW_IF_FAILED(
        m_commandAllocators[r_frameIndex]->Reset(),
        "Unable to reset command allocator.  Its associated memory may still be in use."
    );

    // Reset our command list to prepare it for new commands.
    THROW_IF_FAILED(
        m_commandList->Reset(m_commandAllocators[r_frameIndex].Get(), nullptr),
        "Unable to reset command list.  It may not have been closed or submitted properly."
    );
}


void PipelineClass::Close()
{
    // Close the command list so it can be submitted to a command queue.
    THROW_IF_FAILED(
        m_commandList->Close(),
        "Unable to close command list.  It may not have been reset properly."
    );
}


void PipelineClass::AddBarrier(const D3D12_RESOURCE_BARRIER barrier)
{
    // Only need to add the barrier.
    m_commandList->ResourceBarrier(1, &barrier);
}


void PipelineClass::SetState(ID3D12PipelineState* state)
{
    m_commandList->SetPipelineState(state);
}


void PipelineClass::NameD3DResources()
{
    // Name all DirectX objects.
    m_commandList->SetName(L"PC command list");
    for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
    {
        std::wstring name = L"PC command list " + std::to_wstring(i);
        m_commandAllocators[i]->SetName(name.c_str());
    }
}
