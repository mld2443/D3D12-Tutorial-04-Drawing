////////////////////////////////////////////////////////////////////////////////
// Filename: graphicspipelineinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "pipelineinterface.h"


////////////////////////////////////////////////////////////////////////////////
// Interface name: GraphicsPipelineInterface
////////////////////////////////////////////////////////////////////////////////
class GraphicsPipelineInterface : public PipelineInterface
{
public:
	GraphicsPipelineInterface() = delete;
	GraphicsPipelineInterface(const GraphicsPipelineInterface&) = delete;
	GraphicsPipelineInterface& operator=(const GraphicsPipelineInterface&) = delete;

	GraphicsPipelineInterface(ID3D12Device*);
	~GraphicsPipelineInterface();

	void OpenPipeline(UINT);
	void ClosePipeline();

	ID3D12GraphicsCommandList* GetCommandList() override;

protected:
	virtual void InitializeRootSignature(ID3D12Device*) = 0;
	void InitializePipeline(ID3D12Device*) override;
	virtual void InitializeViewport(UINT, UINT);

	virtual void SetShaderBytecode() = 0;
	virtual void SetBlendDesc();
	virtual void SetRasterDesc();
	virtual void SetDepthStencilDesc();
	virtual void SetInputLayoutDesc() = 0;
	virtual void InitializePipelineStateObject(ID3D12Device*);

	void UpdateConstantBuffer(UINT, BYTE*, SIZE_T) override;

	virtual void NameD3D12Resources() = 0;

protected:
	ID3D12GraphicsCommandList*	m_graphicsCommands;

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
