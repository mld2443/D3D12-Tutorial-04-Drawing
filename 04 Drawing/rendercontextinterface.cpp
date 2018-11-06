////////////////////////////////////////////////////////////////////////////////
// Filename: rendercontextinterface.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "rendercontextinterface.h"


void RenderContextInterface::InitializeContext(ID3D12Device* device)
{
	// Check that the root signature is properly set up before using.
	THROW_IF_FALSE(
		m_rootSignature,
		"Interface failed to initialize root signature correctly."
	);

	// First we need to set all the descriptions for the pipeline.
	SetShaderBytecode();
	SetBlendDesc();
	SetRasterDesc();
	SetDepthStencilDesc();
	SetInputLayoutDesc();

	// Then we can initialize the pipeline state and parameters.
	InitializeState(device);
}


void RenderContextInterface::InitializeViewport(UINT screenWidth, UINT screenHeight)
{
	// Set up the viewport for rendering.
	m_viewport.Width =		(float)screenWidth;
	m_viewport.Height =		(float)screenHeight;
	m_viewport.MinDepth =	D3D12_DEFAULT_VIEWPORT_MIN_DEPTH;
	m_viewport.MaxDepth =	1.0f;
	m_viewport.TopLeftX =	D3D12_DEFAULT_VIEWPORT_TOPLEFTX;
	m_viewport.TopLeftY =	D3D12_DEFAULT_VIEWPORT_TOPLEFTY;

	// Set up the scissor rect for the viewport.
	m_scissorRect.left =	D3D12_DEFAULT_SCISSOR_STARTX;
	m_scissorRect.top =		D3D12_DEFAULT_SCISSOR_STARTY;
	m_scissorRect.right =	screenWidth;
	m_scissorRect.bottom =	screenHeight;
}


void RenderContextInterface::InitializeState(ID3D12Device* device)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;


	// Set up the Pipeline State for this render pipeline.
	ZeroMemory(&pipelineStateDesc, sizeof(pipelineStateDesc));
	pipelineStateDesc.pRootSignature =					m_rootSignature.Get();
	pipelineStateDesc.VS =								m_vsBytecode;
	pipelineStateDesc.HS =								m_hsBytecode;
	pipelineStateDesc.DS =								m_dsBytecode;
	pipelineStateDesc.GS =								m_gsBytecode;
	pipelineStateDesc.PS =								m_psBytecode;
	pipelineStateDesc.BlendState =						m_blendDesc;
	pipelineStateDesc.SampleMask =						D3D12_DEFAULT_SAMPLE_MASK;
	pipelineStateDesc.RasterizerState =					m_rasterDesc;
	pipelineStateDesc.DepthStencilState =				m_depthStencilDesc;
	pipelineStateDesc.InputLayout.NumElements =			static_cast<UINT>(m_inputLayoutDesc.size());
	pipelineStateDesc.InputLayout.pInputElementDescs =	m_inputLayoutDesc.data();
	pipelineStateDesc.PrimitiveTopologyType =			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets =				1;
	pipelineStateDesc.RTVFormats[0] =					DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DSVFormat =						DXGI_FORMAT_D32_FLOAT;
	pipelineStateDesc.SampleDesc.Count =				1;
	pipelineStateDesc.SampleDesc.Quality =				0;
	pipelineStateDesc.NodeMask =						0;
	pipelineStateDesc.Flags =							D3D12_PIPELINE_STATE_FLAG_NONE;

	// Create the pipeline state object.
	THROW_IF_FAILED(
		device->CreateGraphicsPipelineState(
			&pipelineStateDesc,
			IID_PPV_ARGS(m_state.ReleaseAndGetAddressOf())),
		"The pipeline state object failed to initialize."
	);
}


void RenderContextInterface::SetBlendDesc()
{
	// Create an alpha enabled blend state description.
	m_blendDesc.AlphaToCoverageEnable =					FALSE;
	m_blendDesc.IndependentBlendEnable =				FALSE;
	m_blendDesc.RenderTarget[0].BlendEnable =			TRUE;
	m_blendDesc.RenderTarget[0].LogicOpEnable =			FALSE;
	m_blendDesc.RenderTarget[0].SrcBlend =				D3D12_BLEND_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].DestBlend =				D3D12_BLEND_INV_SRC_ALPHA;
	m_blendDesc.RenderTarget[0].BlendOp =				D3D12_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].SrcBlendAlpha =			D3D12_BLEND_ONE;
	m_blendDesc.RenderTarget[0].DestBlendAlpha =		D3D12_BLEND_ZERO;
	m_blendDesc.RenderTarget[0].BlendOpAlpha =			D3D12_BLEND_OP_ADD;
	m_blendDesc.RenderTarget[0].RenderTargetWriteMask =	0x0f;
}


void RenderContextInterface::SetRasterDesc()
{
	// Setup the raster description which will determine how and what polygons will be drawn.
	m_rasterDesc.FillMode =					D3D12_FILL_MODE_SOLID;
	m_rasterDesc.CullMode =					D3D12_CULL_MODE_BACK;
	m_rasterDesc.FrontCounterClockwise =	false;
	m_rasterDesc.DepthBias =				D3D12_DEFAULT_DEPTH_BIAS;
	m_rasterDesc.DepthBiasClamp =			D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	m_rasterDesc.SlopeScaledDepthBias =		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	m_rasterDesc.DepthClipEnable =			true;
	m_rasterDesc.MultisampleEnable =		false;
	m_rasterDesc.AntialiasedLineEnable =	false;
	m_rasterDesc.ForcedSampleCount =		0;
	m_rasterDesc.ConservativeRaster =		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}


void RenderContextInterface::SetDepthStencilDesc()
{
	// Set up the description of the stencil state.
	m_depthStencilDesc.DepthEnable =		true;
	m_depthStencilDesc.DepthWriteMask =		D3D12_DEPTH_WRITE_MASK_ALL;
	m_depthStencilDesc.DepthFunc =			D3D12_COMPARISON_FUNC_LESS_EQUAL;
	m_depthStencilDesc.StencilEnable =		true;
	m_depthStencilDesc.StencilReadMask =	D3D12_DEFAULT_STENCIL_READ_MASK;
	m_depthStencilDesc.StencilWriteMask =	D3D12_DEFAULT_STENCIL_WRITE_MASK;

	// Stencil operations if pixel is front-facing.
	m_depthStencilDesc.FrontFace.StencilFailOp =		D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilDepthFailOp =	D3D12_STENCIL_OP_INCR;
	m_depthStencilDesc.FrontFace.StencilPassOp =		D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilFunc =			D3D12_COMPARISON_FUNC_ALWAYS;

	// Stencil operations if pixel is back-facing.
	m_depthStencilDesc.BackFace.StencilFailOp =			D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilDepthFailOp =	D3D12_STENCIL_OP_DECR;
	m_depthStencilDesc.BackFace.StencilPassOp =			D3D12_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilFunc =			D3D12_COMPARISON_FUNC_ALWAYS;
}
