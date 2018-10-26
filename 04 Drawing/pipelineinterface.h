////////////////////////////////////////////////////////////////////////////////
// Filename: pipelineinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>


/////////////////
// DEFINITIONS //
/////////////////
#define FRAME_BUFFER_COUNT 2
#define PI 3.141592654f

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Interface name: PipelineInterface
////////////////////////////////////////////////////////////////////////////////
class PipelineInterface
{
public:
	PipelineInterface();
	PipelineInterface(const PipelineInterface&);
	~PipelineInterface();

	virtual bool Initialize(ID3D12Device*, HWND, unsigned int, int, int, float, float) = 0;
	virtual void Shutdown() = 0;

	bool OpenPipeline(unsigned int);
	bool ClosePipeline();

	virtual bool SetPipelineParameters(unsigned int, XMMATRIX) = 0;

	XMMATRIX GetProjectionMatrix();
	XMMATRIX GetWorldMatrix();
	XMMATRIX GetOrthoMatrix();
	ID3D12GraphicsCommandList* GetCommandList();

protected:
	virtual bool InitializeRootSignature(ID3D12Device*) = 0;
	bool InitializePipeline(ID3D12Device*);
	bool InitializeCommandList(ID3D12Device*, unsigned int);
	virtual void InitializeViewport(int, int, float, float);
	void ShutdownPipeline();
	void ShutdownCommandList();

	virtual void SetShaderBytecode() = 0;
	virtual void SetBlendDesc();
	virtual void SetRasterDesc();
	virtual void SetDepthStencilDesc();
	virtual void SetInputLayoutDesc() = 0;
	virtual bool InitializePipelineStateObject(ID3D12Device*);

protected:
	ID3D12RootSignature*		m_rootSignature;
	ID3D12PipelineState*		m_pipelineState;
	ID3D12CommandAllocator*		m_commandAllocator[FRAME_BUFFER_COUNT];
	ID3D12GraphicsCommandList*	m_commandList;

	D3D12_SHADER_BYTECODE					m_vsBytecode, m_psBytecode;
	D3D12_BLEND_DESC						m_blendDesc;
	D3D12_RASTERIZER_DESC					m_rasterDesc;
	D3D12_DEPTH_STENCIL_DESC				m_depthStencilDesc;
	std::vector<D3D12_INPUT_ELEMENT_DESC>	m_inputLayoutDesc;

	XMMATRIX	m_projectionMatrix;
	XMMATRIX	m_worldMatrix;
	XMMATRIX	m_orthoMatrix;

	D3D12_VIEWPORT	m_viewport;
	D3D12_RECT		m_scissorRect;
};

