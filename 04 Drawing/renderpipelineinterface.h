////////////////////////////////////////////////////////////////////////////////
// Filename: renderpipelineinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "pipelineinterface.h"


////////////////////////////////////////////////////////////////////////////////
// Interface name: RenderPipelineInterface
////////////////////////////////////////////////////////////////////////////////
class RenderPipelineInterface : public PipelineInterface
{
public:
	RenderPipelineInterface() = delete;
	RenderPipelineInterface(const RenderPipelineInterface&) = delete;
	RenderPipelineInterface& operator=(const RenderPipelineInterface&) = delete;

	RenderPipelineInterface(ID3D12Device*);
	~RenderPipelineInterface() = default;

protected:
	virtual void InitializeRootSignature(ID3D12Device*) = 0;
	void InitializePipeline(ID3D12Device*) override;
	virtual void InitializeViewport(UINT, UINT);
	virtual void InitializeStateObject(ID3D12Device*) override;

	virtual void SetShaderBytecode() = 0;
	virtual void SetBlendDesc();
	virtual void SetRasterDesc();
	virtual void SetDepthStencilDesc();
	virtual void SetInputLayoutDesc() = 0;

	void UpdateConstantBuffer(UINT, BYTE*, SIZE_T) override;

	virtual void NameD3D12Resources() = 0;

protected:
	D3D12_SHADER_BYTECODE				m_vsBytecode =			D3D12_SHADER_BYTECODE();
	D3D12_SHADER_BYTECODE				m_hsBytecode =			D3D12_SHADER_BYTECODE();
	D3D12_SHADER_BYTECODE				m_dsBytecode =			D3D12_SHADER_BYTECODE();
	D3D12_SHADER_BYTECODE				m_gsBytecode =			D3D12_SHADER_BYTECODE();
	D3D12_SHADER_BYTECODE				m_psBytecode =			D3D12_SHADER_BYTECODE();
	D3D12_BLEND_DESC					m_blendDesc =			D3D12_BLEND_DESC();
	D3D12_RASTERIZER_DESC				m_rasterDesc =			D3D12_RASTERIZER_DESC();
	D3D12_DEPTH_STENCIL_DESC			m_depthStencilDesc =	D3D12_DEPTH_STENCIL_DESC();
	vector<D3D12_INPUT_ELEMENT_DESC>	m_inputLayoutDesc =		vector<D3D12_INPUT_ELEMENT_DESC>();

	D3D12_VIEWPORT	m_viewport =	D3D12_VIEWPORT();
	D3D12_RECT		m_scissorRect =	D3D12_RECT();
};
