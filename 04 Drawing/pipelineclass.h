////////////////////////////////////////////////////////////////////////////////
// Filename: pipelineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


////////////////////////////////////////////////////////////////////////////////
// Class name: PipelineClass
////////////////////////////////////////////////////////////////////////////////
class PipelineClass
{
public:
    PipelineClass(ID3D12Device *, const UINT &, D3D12_COMMAND_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT);
    ~PipelineClass() = default;

    ID3D12GraphicsCommandList * GetCommandList();

    void Open();
    void Close();

    void AddBarrier(const D3D12_RESOURCE_BARRIER);
    void SetState(ID3D12PipelineState *);

private:
    void NameD3DResources();

private:
    const UINT &r_frameIndex;

    std::array<ComPtr<ID3D12CommandAllocator>, FRAME_BUFFER_COUNT> m_commandAllocators = {};
    ComPtr<ID3D12GraphicsCommandList>                              m_commandList       = nullptr;
};
