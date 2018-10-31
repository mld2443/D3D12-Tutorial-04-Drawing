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

	void OpenPipeline(UINT);
	void ClosePipeline();

	virtual void SetPipelineParameters(UINT, XMMATRIX, XMMATRIX) = 0;

	XMMATRIX GetWorldMatrix();
	XMMATRIX GetOrthoMatrix();
	ID3D12GraphicsCommandList* GetCommandList();

protected:
	void InitializePipeline(ID3D12Device*);
	void InitializeCommandList(ID3D12Device*, UINT);
	virtual void InitializeViewport(UINT, UINT, float, float);

	virtual void SetShaderBytecode() = 0;
	virtual void SetBlendDesc();
	virtual void SetRasterDesc();
	virtual void SetDepthStencilDesc();
	virtual void SetInputLayoutDesc() = 0;
	virtual void InitializePipelineStateObject(ID3D12Device*);

protected:
	ID3D12RootSignature*			m_rootSignature =		nullptr;
	ID3D12PipelineState*			m_pipelineState =		nullptr;
	vector<ID3D12CommandAllocator*>	m_commandAllocators =	vector<ID3D12CommandAllocator*>(FRAME_BUFFER_COUNT, nullptr);
	ID3D12GraphicsCommandList*		m_commandList =			nullptr;

	D3D12_SHADER_BYTECODE				m_vsBytecode =			D3D12_SHADER_BYTECODE();
	D3D12_SHADER_BYTECODE				m_psBytecode =			D3D12_SHADER_BYTECODE();
	D3D12_BLEND_DESC					m_blendDesc =			D3D12_BLEND_DESC();
	D3D12_RASTERIZER_DESC				m_rasterDesc =			D3D12_RASTERIZER_DESC();
	D3D12_DEPTH_STENCIL_DESC			m_depthStencilDesc =	D3D12_DEPTH_STENCIL_DESC();
	vector<D3D12_INPUT_ELEMENT_DESC>	m_inputLayoutDesc =		vector<D3D12_INPUT_ELEMENT_DESC>();

	XMMATRIX	m_worldMatrix =	XMMatrixIdentity();
	XMMATRIX	m_orthoMatrix = XMMatrixIdentity();

	D3D12_VIEWPORT	m_viewport =	D3D12_VIEWPORT();
	D3D12_RECT		m_scissorRect =	D3D12_RECT();
};

