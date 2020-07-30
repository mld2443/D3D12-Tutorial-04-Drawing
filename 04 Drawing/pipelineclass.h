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
    enum CommandType
    {
        open,
        close
    };

    PipelineClass(ID3D12Device*, UINT&, D3D12_COMMAND_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT);
    ~PipelineClass() = default;

    ID3D12GraphicsCommandList* GetCommandList();

    void Open();
    void Close();

    void AddBarrier(const D3D12_RESOURCE_BARRIER);

    friend std::unique_ptr<PipelineClass>& operator<<(std::unique_ptr<PipelineClass>&, PipelineClass::CommandType);
    friend std::unique_ptr<PipelineClass>& operator<<(std::unique_ptr<PipelineClass>&, const pipeline_func&);

private:
    void NameD3DResources();

private:
    UINT& r_frameIndex;

    vector<ComPtr<ID3D12CommandAllocator>> m_commandAllocators = vector<ComPtr<ID3D12CommandAllocator>>(FRAME_BUFFER_COUNT, nullptr);
    ComPtr<ID3D12GraphicsCommandList>      m_commandList       = nullptr;
};
