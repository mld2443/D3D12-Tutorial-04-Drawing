////////////////////////////////////////////////////////////////////////////////
// Filename: pipelineinterface.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pipelineinterface.h"


PipelineInterface::PipelineInterface()
{
	m_rootSignature = nullptr;
	m_pipelineState = nullptr;
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_commandAllocator[i] = nullptr;
	}
	m_commandList = nullptr;
}


PipelineInterface::PipelineInterface(const PipelineInterface& other)
{
}


PipelineInterface::~PipelineInterface()
{
}


bool PipelineInterface::OpenPipeline(unsigned int frameIndex)
{
	HRESULT result;


	// Reset the memory that was holding the previously submitted command list.
	result = m_commandAllocator[frameIndex]->Reset();
	if (FAILED(result))
	{
		return false;
	}

	// Reset our command list to prepare it for new commands.
	result = m_commandList->Reset(m_commandAllocator[frameIndex], m_pipelineState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


bool PipelineInterface::ClosePipeline()
{
	HRESULT result;


	// Close the command list so it can be submitted to a command queue.
	result = m_commandList->Close();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


XMMATRIX PipelineInterface::GetProjectionMatrix()
{
	return m_projectionMatrix;
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


bool PipelineInterface::InitializePipeline(ID3D12Device* device)
{
	bool result;


	//TODO: Check that the root signature is set up. Maybe remove the InitializeRootSignature pure function.

	// First we need to set all the descriptions for the pipeline.
	SetShaderBytecode();
	SetBlendDesc();
	SetRasterDesc();
	SetDepthStencilDesc();
	SetInputLayoutDesc();

	// Then we can initialize the pipeline state and parameters.
	result = InitializePipelineStateObject(device);
	if (!result)
	{
		return false;
	}

	return true;
}


bool PipelineInterface::InitializeCommandList(ID3D12Device* device, unsigned int frameIndex)
{
	HRESULT result;


	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		// Create command allocators, one for each frame.
		result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator[i]));
		if (FAILED(result))
		{
			return false;
		}
	}

	// Create a command list.
	result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[frameIndex], nullptr, IID_PPV_ARGS(&m_commandList));
	if (FAILED(result))
	{
		return false;
	}

	// Initially we need to close the command list during initialization as it is created in a recording state.
	result = m_commandList->Close();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void PipelineInterface::InitializeViewport(int screenWidth, int screenHeight, float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;


	// Set up the projection matrix.
	fieldOfView = PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//TODO: The projection matrix feels like it belongs in the camera class.
	// Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// Initialize the world matrix to the identity matrix.
	m_worldMatrix = XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// Set up the viewport for rendering.
	ZeroMemory(&m_viewport, sizeof(m_viewport));
	m_viewport.Width =		(float)screenWidth;
	m_viewport.Height =		(float)screenHeight;
	m_viewport.MinDepth =	0.0f;
	m_viewport.MaxDepth =	1.0f;
	m_viewport.TopLeftX =	0.0f;
	m_viewport.TopLeftY =	0.0f;

	// Set up the scissor rect for the viewport.
	ZeroMemory(&m_scissorRect, sizeof(m_scissorRect));
	m_scissorRect.left =	0;
	m_scissorRect.top =		0;
	m_scissorRect.right =	screenWidth;
	m_scissorRect.bottom =	screenHeight;

	return;
}


void PipelineInterface::ShutdownPipeline()
{
	// Release the pipeline state object.
	if (m_pipelineState)
	{
		m_pipelineState->Release();
		m_pipelineState = nullptr;
	}

	// Release the root signature.
	if (m_rootSignature)
	{
		m_rootSignature->Release();
		m_rootSignature = nullptr;
	}

	return;
}


void PipelineInterface::ShutdownCommandList()
{
	// Release the command list.
	if (m_commandList)
	{
		m_commandList->Release();
		m_commandList = nullptr;
	}

	// Release the command allocators.
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		if (m_commandAllocator[i])
		{
			m_commandAllocator[i]->Release();
			m_commandAllocator[i] = nullptr;
		}
	}

	return;
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

	return;
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

	return;
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

	return;
}


bool PipelineInterface::InitializePipelineStateObject(ID3D12Device* device)
{
	HRESULT result;
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
	pipelineStateDesc.SampleDesc.Count =				1;
	pipelineStateDesc.SampleDesc.Quality =				0;
	pipelineStateDesc.Flags =							D3D12_PIPELINE_STATE_FLAG_NONE;

	// Create the pipeline state.
	result = device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(result))
	{
		return false;
	}

	return true;
}
