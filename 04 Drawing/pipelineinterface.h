////////////////////////////////////////////////////////////////////////////////
// Filename: pipelineinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


////////////////////////////////////////////////////////////////////////////////
// Class name: PipelineInterface
////////////////////////////////////////////////////////////////////////////////
class PipelineInterface
{
public:
	PipelineInterface() = delete;
	PipelineInterface(const PipelineInterface&) = delete;
	PipelineInterface& operator=(const PipelineInterface&) = delete;

	PipelineInterface(ID3D12Device*, D3D12_COMMAND_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT);
	~PipelineInterface() = default;

	ID3D12GraphicsCommandList* GetCommandList();

	void OpenPipeline(UINT, ID3D12PipelineState* = nullptr);
	void ClosePipeline();

	friend PipelineInterface& operator<<(PipelineInterface&, ID3D12PipelineState*);
	friend PipelineInterface& operator<<(PipelineInterface&, D3D12_RESOURCE_BARRIER&);

private:
	vector<ComPtr<ID3D12CommandAllocator>>	m_commandAllocators = vector<ComPtr<ID3D12CommandAllocator>>(FRAME_BUFFER_COUNT, nullptr);
	ComPtr<ID3D12GraphicsCommandList>		m_commandList = nullptr;
};
