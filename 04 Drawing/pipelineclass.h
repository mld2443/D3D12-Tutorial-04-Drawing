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

	PipelineClass() = delete;
	PipelineClass(const PipelineClass&) = delete;
	PipelineClass& operator=(const PipelineClass&) = delete;

	PipelineClass(ID3D12Device*, UINT&, D3D12_COMMAND_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT);
	~PipelineClass() = default;

	ID3D12GraphicsCommandList* GetCommandList();


	friend PipelineClass& operator<<(PipelineClass&, PipelineClass::CommandType);
	friend PipelineClass& operator<<(PipelineClass&, ID3D12PipelineState*);
	friend PipelineClass& operator<<(PipelineClass&, D3D12_RESOURCE_BARRIER&);

private:
	void Open();
	void Close();

private:
	UINT&	r_frameIndex;

	vector<ComPtr<ID3D12CommandAllocator>>	m_commandAllocators = vector<ComPtr<ID3D12CommandAllocator>>(FRAME_BUFFER_COUNT, nullptr);
	ComPtr<ID3D12GraphicsCommandList>		m_commandList = nullptr;
};
