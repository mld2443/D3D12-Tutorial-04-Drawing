////////////////////////////////////////////////////////////////////////////////
// Filename: pipelineinterface.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "pipelineinterface.h"


PipelineInterface::~PipelineInterface()
{
	// Release reserved resources.
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		SAFE_RELEASE(m_commandAllocators[i]);
	}
	SAFE_RELEASE(m_commandList);
	SAFE_RELEASE(m_pipelineState);
	SAFE_RELEASE(m_rootSignature);
}


void PipelineInterface::OpenPipeline(UINT frameIndex)
{
	// Reset the memory that was holding the previously submitted command list.
	THROW_IF_FAILED(m_commandAllocators[frameIndex]->Reset(),
		L"Unable to reset command allocator.  Its associated memory may still be in use.",
		L"Pipeline Access Error");

	// Reset our command list to prepare it for new commands.
	THROW_IF_FAILED(m_commandList->Reset(m_commandAllocators[frameIndex], m_pipelineState),
		L"Unable to reset command list.  It may not have been closed or submitted properly.",
		L"Command List Reset Error");
}


void PipelineInterface::ClosePipeline()
{
	// Close the command list so it can be submitted to a command queue.
	THROW_IF_FAILED(m_commandList->Close(),
		L"Unable to close command list.  It may not have been reset properly.",
		L"Command List Close Error");
}


XMMATRIX PipelineInterface::GetWorldMatrix()
{
	return m_worldMatrix;
}


XMMATRIX PipelineInterface::GetOrthoMatrix()
{
	return m_orthoMatrix;
}


ID3D12GraphicsCommandList* PipelineInterface::GetCommandList()
{
	return m_commandList;
}


void PipelineInterface::InitializePipeline(ID3D12Device* device)
{
	// Check that the root signature is properly set up before using.
	THROW_IF_FALSE(m_rootSignature,
		L"Pipeline failed to initialize root signature correctly.",
		L"Graphics Pipeline Initialization Failure");

	// First we need to set all the descriptions for the pipeline.
	SetShaderBytecode();
	SetBlendDesc();
	SetRasterDesc();
	SetDepthStencilDesc();
	SetInputLayoutDesc();

	// Then we can initialize the pipeline state and parameters.
	InitializePipelineStateObject(device);
}


void PipelineInterface::InitializeCommandList(ID3D12Device* device, UINT frameIndex)
{
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		// Create command allocators, one for each frame.
		THROW_IF_FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i])),
			L"Unable to create the command allocator object.",
			L"Graphics Pipeline Initialization Failure");
	}

	// Create a command list.
	THROW_IF_FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[frameIndex], nullptr, IID_PPV_ARGS(&m_commandList)),
		L"Unable to create the command list object.",
		L"Graphics Pipeline Initialization Failure");

	// Initially we need to close the command list during initialization as it is created in a recording state.
	THROW_IF_FAILED(m_commandList->Close(),
		L"Unable to close command list after creation.",
		L"Graphics Pipeline Initialization Failure");
}


void PipelineInterface::InitializeViewport(UINT screenWidth, UINT screenHeight, float screenNear, float screenFar)
{
	// Initialize the world matrix to the identity matrix.
	m_worldMatrix = XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenFar);

	// Set up the viewport for rendering.
	ZeroMemory(&m_viewport, sizeof(m_viewport));
	m_viewport.Width =		(float)screenWidth;
	m_viewport.Height =		(float)screenHeight;
	m_viewport.MinDepth =	D3D12_DEFAULT_VIEWPORT_MIN_DEPTH;
	m_viewport.MaxDepth =	1.0f;
	m_viewport.TopLeftX =	D3D12_DEFAULT_VIEWPORT_TOPLEFTX;
	m_viewport.TopLeftY =	D3D12_DEFAULT_VIEWPORT_TOPLEFTY;

	// Set up the scissor rect for the viewport.
	ZeroMemory(&m_scissorRect, sizeof(m_scissorRect));
	m_scissorRect.left =	D3D12_DEFAULT_SCISSOR_STARTX;
	m_scissorRect.top =		D3D12_DEFAULT_SCISSOR_STARTY;
	m_scissorRect.right =	screenWidth;
	m_scissorRect.bottom =	screenHeight;
}


void PipelineInterface::SetBlendDesc()
{
	// Create an alpha enabled blend state description.
	ZeroMemory(&m_blendDesc, sizeof(m_blendDesc));
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


void PipelineInterface::SetRasterDesc()
{
	// Setup the raster description which will determine how and what polygons will be drawn.
	ZeroMemory(&m_rasterDesc, sizeof(m_rasterDesc));
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


void PipelineInterface::SetDepthStencilDesc()
{
	// Set up the description of the stencil state.
	ZeroMemory(&m_depthStencilDesc, sizeof(m_depthStencilDesc));
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


void PipelineInterface::InitializePipelineStateObject(ID3D12Device* device)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;


	// Set up the Pipeline State for this render pipeline.
	ZeroMemory(&pipelineStateDesc, sizeof(pipelineStateDesc));
	pipelineStateDesc.pRootSignature =					m_rootSignature;
	pipelineStateDesc.VS =								m_vsBytecode;
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
	pipelineStateDesc.Flags =							D3D12_PIPELINE_STATE_FLAG_NONE;

	// Create the pipeline state.
	THROW_IF_FAILED(device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&m_pipelineState)),
		L"The pipeline state object failed to initialize.",
		L"Pipeline Initializer Failure");
}
