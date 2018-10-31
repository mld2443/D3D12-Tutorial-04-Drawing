////////////////////////////////////////////////////////////////////////////////
// Filename: pipelineinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


////////////////////////////////////////////////////////////////////////////////
// Interface name: PipelineInterface
////////////////////////////////////////////////////////////////////////////////
class PipelineInterface
{
public:
	PipelineInterface(const PipelineInterface&) = delete;
	PipelineInterface& operator=(const PipelineInterface&) = delete;

	PipelineInterface() = default;
	~PipelineInterface();

	virtual ID3D12CommandList* GetCommandList();

protected:
	void InitializeConstantBuffer(ID3D12Device*);
	virtual void InitializeRootSignature(ID3D12Device*) = 0;
	virtual void InitializePipeline(ID3D12Device*) = 0;
	void InitializeCommandList(ID3D12Device*);

	virtual void SetShaderBytecode() = 0;

	virtual void UpdateConstantBuffer(UINT, BYTE*, SIZE_T) = 0;
	D3D12_GPU_VIRTUAL_ADDRESS SetConstantBuffer(UINT, BYTE*, SIZE_T);

	virtual void NameD3D12Resources() = 0;

protected:
	ID3D12RootSignature*			m_rootSignature =		nullptr;
	ID3D12PipelineState*			m_pipelineState =		nullptr;
	vector<ID3D12CommandAllocator*>	m_commandAllocators =	vector<ID3D12CommandAllocator*>(FRAME_BUFFER_COUNT, nullptr);
	ID3D12CommandList*				m_commandList =			nullptr;
	ID3D12Resource*					m_constantBuffer =		nullptr;

	SIZE_T	m_constantBufferWidth = 0;
};
