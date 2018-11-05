////////////////////////////////////////////////////////////////////////////////
// Filename: contextinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


////////////////////////////////////////////////////////////////////////////////
// Interface name: ContextInterface
////////////////////////////////////////////////////////////////////////////////
class ContextInterface
{
public:
	ContextInterface() = delete;
	ContextInterface(const ContextInterface&) = delete;
	ContextInterface& operator=(const ContextInterface&) = delete;

	ContextInterface(ID3D12Device*, D3D12_COMMAND_LIST_TYPE);
	~ContextInterface() = default;

	ID3D12GraphicsCommandList* GetCommandList();

	void OpenPipeline(UINT);
	void ClosePipeline();

protected:
	virtual void InitializeRootSignature(ID3D12Device*) = 0;
	virtual void InitializeConstantBuffer(ID3D12Device*);
	virtual void InitializeContext(ID3D12Device*) = 0;
	virtual void InitializeStateObject(ID3D12Device*) = 0;

	virtual void SetShaderBytecode() = 0;

	virtual void UpdateConstantBuffer(UINT, BYTE*, SIZE_T) = 0;
	D3D12_GPU_VIRTUAL_ADDRESS SetConstantBuffer(UINT, BYTE*, SIZE_T);

	virtual void NameD3DResources() = 0;

protected:
	ComPtr<ID3D12RootSignature>				m_rootSignature =		nullptr;
	ComPtr<ID3D12PipelineState>				m_pipelineState =		nullptr;
	vector<ComPtr<ID3D12CommandAllocator>>	m_commandAllocators =	vector<ComPtr<ID3D12CommandAllocator>>(FRAME_BUFFER_COUNT, nullptr);
	ComPtr<ID3D12GraphicsCommandList>		m_commandList =			nullptr;
	ComPtr<ID3D12Resource>					m_constantBuffer =		nullptr;

	SIZE_T	m_constantBufferWidth = 0;
};
