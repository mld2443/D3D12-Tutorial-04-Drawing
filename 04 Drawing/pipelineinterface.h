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
#define QUARTER_PI 3.141592654f / 4.0f

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

	virtual bool Initialize(ID3D12Device*, HWND, unsigned int, int, int, float, float);
	virtual void Shutdown();

	virtual bool UpdatePipeline(unsigned int) = 0;
	virtual ID3D12CommandList* ClosePipeline() = 0;

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);
	ID3D12GraphicsCommandList* GetCommandList();

protected:
	virtual bool InitializeRootSignature(ID3D12Device*) = 0;
	virtual void SetShaderBytecode() = 0;
	virtual void SetBlendDesc();
	virtual void SetRasterDesc();
	virtual void SetDepthStencilDesc();
	virtual std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputLayoutDesc() = 0;
	virtual bool InitializePipelineState(ID3D12Device*);

	bool InitializeCommandList(ID3D12Device*, unsigned int, int, int);
	void ShutdownCommandList();

protected:
	ID3D12RootSignature*		m_rootSignature;
	ID3D12PipelineState*		m_pipelineState;
	ID3D12CommandAllocator*		m_commandAllocator[FRAME_BUFFER_COUNT];
	ID3D12GraphicsCommandList*	m_commandList;

	D3D12_SHADER_BYTECODE		m_vsBytecode, m_psBytecode;
	D3D12_BLEND_DESC			m_blendDesc;
	D3D12_RASTERIZER_DESC		m_rasterDesc;
	D3D12_DEPTH_STENCIL_DESC	m_depthStencilDesc;

	XMMATRIX	m_projectionMatrix;
	XMMATRIX	m_worldMatrix;
	XMMATRIX	m_orthoMatrix;

	D3D12_VIEWPORT	m_viewport;
};

