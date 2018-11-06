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
	PipelineClass() = delete;
	PipelineClass(const PipelineClass&) = delete;
	PipelineClass& operator=(const PipelineClass&) = delete;

	PipelineClass(ID3D12Device*, D3D12_COMMAND_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT);
	~PipelineClass() = default;

	ID3D12GraphicsCommandList* GetCommandList();

	void OpenPipeline(UINT, ID3D12PipelineState* = nullptr);
	void ClosePipeline();

	friend PipelineClass& operator<<(PipelineClass&, ID3D12PipelineState*);
	friend PipelineClass& operator<<(PipelineClass&, D3D12_RESOURCE_BARRIER&);

private:
	vector<ComPtr<ID3D12CommandAllocator>>	m_commandAllocators = vector<ComPtr<ID3D12CommandAllocator>>(FRAME_BUFFER_COUNT, nullptr);
	ComPtr<ID3D12GraphicsCommandList>		m_commandList = nullptr;
};
