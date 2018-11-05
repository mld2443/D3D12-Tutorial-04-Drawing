////////////////////////////////////////////////////////////////////////////////
// Filename: instancecontextclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "instancecontextclass.h"


InstanceContextClass::InstanceContextClass(ID3D12Device* device, UINT screenWidth, UINT screenHeight, float screenNear, float screenFar) :
	RenderContextInterface(device),
	m_matrixBuffer(device, sizeof(MatrixBufferType)),
	m_orthoMatrix(XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenFar))
{
	// We need to set up the root signature before creating the pipeline state object.
	InitializeRootSignature(device);

	// Then we can initialize the pipeline state and parameters.
	InitializeContext(device);

	// Initialize the viewport and scissor rectangle.
	InitializeViewport(screenWidth, screenHeight);

	// After all the resources are initialized, we will name all of our objects for graphics debugging.
	NameD3DResources();
}


XMMATRIX InstanceContextClass::GetWorldMatrix()
{
	return m_worldMatrix;
}


XMMATRIX InstanceContextClass::GetOrthoMatrix()
{
	return m_orthoMatrix;
}


void InstanceContextClass::SetPipelineParameters(UINT frameIndex, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	MatrixBufferType matrices;
	D3D12_GPU_VIRTUAL_ADDRESS cbvAddress;

	
	// Set the window viewport.
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	// Declare the root signature.
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	// Transpose and copy the matrices into the constant buffer.
	matrices.world = XMMatrixTranspose(m_worldMatrix);
	matrices.view = XMMatrixTranspose(viewMatrix);
	matrices.projection = XMMatrixTranspose(projectionMatrix);

	// Set the data and get the address of the constant buffer for this frame.
	cbvAddress = m_matrixBuffer.SetConstantBuffer(frameIndex, reinterpret_cast<BYTE*>(&matrices));

	// Tell the root descriptor where the data for our matrix buffer is located.
	m_commandList->SetGraphicsRootConstantBufferView(0, cbvAddress);
}


void InstanceContextClass::InitializeRootSignature(ID3D12Device* device)
{
	ComPtr<ID3D10Blob> signature;
	D3D12_ROOT_PARAMETER matrixBufferDesc;
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;


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
			signature.ReleaseAndGetAddressOf(),
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
			IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf())),
		L"Unable to create the root signature for this graphics pipeline.",
		L"Root Signature Initialization Failure"
	);
}


void InstanceContextClass::SetShaderBytecode()
{
	// Create the descriptor for the vertex shader bytecode.
	m_vsBytecode.pShaderBytecode =	g_instancevs;
	m_vsBytecode.BytecodeLength =	sizeof(g_instancevs);

	// Create the descriptor for the pixel shader bytecode.
	m_psBytecode.pShaderBytecode =	g_instanceps;
	m_psBytecode.BytecodeLength =	sizeof(g_instanceps);
}


void InstanceContextClass::SetInputLayoutDesc()
{
	// Set the size of the layout description.
	m_inputLayoutDesc.resize(4);

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

	m_inputLayoutDesc[2].SemanticName =			"POSITION";
	m_inputLayoutDesc[2].SemanticIndex =		1;
	m_inputLayoutDesc[2].Format =				DXGI_FORMAT_R32G32B32_FLOAT;
	m_inputLayoutDesc[2].InputSlot =			1;
	m_inputLayoutDesc[2].AlignedByteOffset =	0;
	m_inputLayoutDesc[2].InputSlotClass =		D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
	m_inputLayoutDesc[2].InstanceDataStepRate =	1;

	m_inputLayoutDesc[3].SemanticName =			"COLOR";
	m_inputLayoutDesc[3].SemanticIndex =		1;
	m_inputLayoutDesc[3].Format =				DXGI_FORMAT_R32G32B32_FLOAT;
	m_inputLayoutDesc[3].InputSlot =			1;
	m_inputLayoutDesc[3].AlignedByteOffset =	D3D12_APPEND_ALIGNED_ELEMENT;
	m_inputLayoutDesc[3].InputSlotClass =		D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
	m_inputLayoutDesc[3].InstanceDataStepRate =	1;
}


void InstanceContextClass::NameD3DResources()
{
	std::wstring name;


	// Name all DirectX objects.
	m_rootSignature->SetName(L"IPC root signature");
	m_pipelineState->SetName(L"IPC pipeline state");
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		name = std::wstring(L"IPC command allocator ") + std::to_wstring(i);
		m_commandAllocators[i]->SetName(name.c_str());
	}
	m_commandList->SetName(L"IPC graphics command list");
	m_matrixBuffer.buffer->SetName(L"IPC matrix buffer");
}
