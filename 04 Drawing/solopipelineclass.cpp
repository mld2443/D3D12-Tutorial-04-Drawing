////////////////////////////////////////////////////////////////////////////////
// Filename: solopipelineclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "solopipelineclass.h"


SoloPipelineClass::SoloPipelineClass(ID3D12Device* device, UINT frameIndex, UINT screenWidth, UINT screenHeight, float screenNear, float screenFar):
	PipelineInterface()
{
	// We need to set up the root signature before creating the pipeline state object.
	InitializeRootSignature(device);

	// Then we can initialize the pipeline state and parameters.
	InitializePipeline(device);

	// Initialize the command list and allocators.
	InitializeCommandList(device, frameIndex);

	// Initialize the viewport and scissor rectangle.
	InitializeViewport(screenWidth, screenHeight, screenNear, screenFar);

	// After all the resources are initialized, we will name all of our objects for graphics debugging.
	NameResources();
}


SoloPipelineClass::~SoloPipelineClass()
{
	SAFE_RELEASE(m_matrixBuffer);
}


void SoloPipelineClass::SetPipelineParameters(UINT frameIndex, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	D3D12_RANGE range;
	BYTE* mappedResource;
	MatrixBufferType* dataPtr;


	// Declare the root signature.
	m_commandList->SetGraphicsRootSignature(m_rootSignature);

	// Create a zero-width read range, [0, 0].
	// This is a signal to the GPU to not worry about the accuracy of the data within.
	ZeroMemory(&range, sizeof(range));

	// Lock the constant buffer so it can be written to.
	THROW_IF_FAILED(
		m_matrixBuffer->Map(0, &range, reinterpret_cast<void**>(&mappedResource)),
		L"Unable to access the memory of the graphics device.",
		L"Graphics Device Communication Failure"
	);

	// Get a pointer to the correct buffer offset location.
	dataPtr = reinterpret_cast<MatrixBufferType*>(&mappedResource[frameIndex * m_matrixBufferWidth]);

	// Transpose and copy the matrices into the constant buffer.
	dataPtr->world = XMMatrixTranspose(m_worldMatrix);
	dataPtr->view = XMMatrixTranspose(viewMatrix);
	dataPtr->projection = XMMatrixTranspose(projectionMatrix);

	// Set the range of data that we wrote to.
	range.Begin =	static_cast<SIZE_T>(frameIndex) * static_cast<SIZE_T>(m_matrixBufferWidth);
	range.End =		range.Begin + m_matrixBufferWidth;

	// Unlock the constant buffer.
	m_matrixBuffer->Unmap(0, &range);

	// Tell the root descriptor where the data for our matrix buffer is located.
	m_commandList->SetGraphicsRootConstantBufferView(0, m_matrixBuffer->GetGPUVirtualAddress() + range.Begin);

	// Set the window viewport.
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);
}


void SoloPipelineClass::InitializeRootSignature(ID3D12Device* device)
{
	UINT64 bufferSize;
	D3D12_HEAP_PROPERTIES heapProps;
	D3D12_RESOURCE_DESC resourceDesc;
	D3D12_ROOT_PARAMETER matrixBufferDesc;
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	ID3D10Blob *signature;


	// Calculate the size of the matrices as they appear in memory.
	m_matrixBufferWidth = (sizeof(MatrixBufferType) + 255) & ~255;

	// Because CPU and GPU are asynchronus, we need to make room for multiple frames worth of matrices.
	bufferSize = static_cast<UINT64>(m_matrixBufferWidth) * FRAME_BUFFER_COUNT;

	// Create description for our constant buffer heap type.
	ZeroMemory(&heapProps, sizeof(heapProps));
	heapProps.Type =					D3D12_HEAP_TYPE_UPLOAD;
	heapProps.CPUPageProperty =			D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference =	D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask =		1;
	heapProps.VisibleNodeMask =			1;

	// Create a description for the memory resource itself.
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension =			D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment =			0;
	resourceDesc.Width =				bufferSize;
	resourceDesc.Height =				1;
	resourceDesc.DepthOrArraySize =		1;
	resourceDesc.MipLevels =			1;
	resourceDesc.Format =				DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count =		1;
	resourceDesc.SampleDesc.Quality =	0;
	resourceDesc.Layout =				D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags =				D3D12_RESOURCE_FLAG_NONE;

	// Allocate the memory on the GPU.
	THROW_IF_FAILED(
		device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_matrixBuffer)),
		L"Unable to allocate space on the graphics device.",
		L"Hardware Memory Allocation Failure"
	);

	// Name the buffer for use while debugging.
	m_matrixBuffer->SetName(L"SPC matrix buffer");

	// Create a descriptor for the matrix buffer.
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.ParameterType =				D3D12_ROOT_PARAMETER_TYPE_CBV;
	matrixBufferDesc.Descriptor.ShaderRegister =	0;
	matrixBufferDesc.Descriptor.RegisterSpace =		0;
	matrixBufferDesc.ShaderVisibility =				D3D12_SHADER_VISIBILITY_VERTEX;

	// Specify which shaders need access to what resources.
	rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
						| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
						| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
						| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
						| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	// Fill out the root signature layout description.
	ZeroMemory(&rootSignatureDesc, sizeof(rootSignatureDesc));
	rootSignatureDesc.NumParameters =		1;
	rootSignatureDesc.pParameters =			&matrixBufferDesc;
	rootSignatureDesc.NumStaticSamplers =	0;
	rootSignatureDesc.pStaticSamplers =		nullptr;
	rootSignatureDesc.Flags =				rootSignatureFlags;

	// Serialize the signature, preparing it for creation on the device.
	THROW_IF_FAILED(
		D3D12SerializeRootSignature(
			&rootSignatureDesc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&signature,
			nullptr),
		L"Unable to serialize the root signature for initialization on the graphics device.",
		L"Root Signature Initialization Failure"
	);

	// Create the root signature on our device.
	THROW_IF_FAILED(
		device->CreateRootSignature(
			0,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(&m_rootSignature)),
		L"Unable to create the root signature for this graphics pipeline.",
		L"Root Signature Initialization Failure"
	);
}


void SoloPipelineClass::SetShaderBytecode()
{
	// Create the descriptor for the vertex shader bytecode.
	ZeroMemory(&m_vsBytecode, sizeof(m_vsBytecode));
	m_vsBytecode.pShaderBytecode =	g_colorvs;
	m_vsBytecode.BytecodeLength =	sizeof(g_colorvs);

	// Create the descriptor for the pixel shader bytecode.
	ZeroMemory(&m_psBytecode, sizeof(m_psBytecode));
	m_psBytecode.pShaderBytecode =	g_colorps;
	m_psBytecode.BytecodeLength =	sizeof(g_colorps);
}


void SoloPipelineClass::SetInputLayoutDesc()
{
	// Set the size of the layout description.
	m_inputLayoutDesc.resize(2);

	// Create the vertex input layout description. This needs to match the VertexType
	// stucture in the geometry interface and the VertexInputType in the shader.
	m_inputLayoutDesc[0].SemanticName =			"POSITION";
	m_inputLayoutDesc[0].SemanticIndex =		0;
	m_inputLayoutDesc[0].Format =				DXGI_FORMAT_R32G32B32_FLOAT;
	m_inputLayoutDesc[0].InputSlot =			0;
	m_inputLayoutDesc[0].AlignedByteOffset =	0;
	m_inputLayoutDesc[0].InputSlotClass =		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	m_inputLayoutDesc[0].InstanceDataStepRate =	0;

	m_inputLayoutDesc[1].SemanticName =			"COLOR";
	m_inputLayoutDesc[1].SemanticIndex =		0;
	m_inputLayoutDesc[1].Format =				DXGI_FORMAT_R32G32B32A32_FLOAT;
	m_inputLayoutDesc[1].InputSlot =			0;
	m_inputLayoutDesc[1].AlignedByteOffset =	D3D12_APPEND_ALIGNED_ELEMENT;
	m_inputLayoutDesc[1].InputSlotClass =		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	m_inputLayoutDesc[1].InstanceDataStepRate =	0;
}


void SoloPipelineClass::NameResources()
{
	std::wstring name;


	// Name all DirectX objects.
	m_rootSignature->SetName(L"SPC root signature");
	m_pipelineState->SetName(L"SPC pipeline state");
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		name = std::wstring(L"SPC command allocator ") + std::to_wstring(i);
		m_commandAllocators[i]->SetName(name.c_str());
	}
	m_commandList->SetName(L"SPC command list");
}
