////////////////////////////////////////////////////////////////////////////////
// Filename: particlecomputeclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "particlecomputeclass.h"


ParticleComputeClass::ParticleComputeClass(ID3D12Device* device) :
	ComputePipelineInterface(device)
{
}


void ParticleComputeClass::InitializeRootSignature(ID3D12Device* device)
{
	D3D12_ROOT_PARAMETER matrixBufferDesc;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	ID3D10Blob *signature;


	// Calculate the size of the matrices as they appear in memory.
	//m_constantBufferWidth = BYTE_ALIGNED_WIDTH(MatrixBufferType, 0xFFu);

	// Create the constant buffer.
	//InitializeConstantBuffer(device);

	D3D12_ROOT_PARAMETER csElapsedTimeCb;
	ZeroMemory(&csElapsedTimeCb, sizeof(csElapsedTimeCb));
	csElapsedTimeCb.ParameterType =				D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	csElapsedTimeCb.Constants.RegisterSpace =	0;
	csElapsedTimeCb.Constants.ShaderRegister =	0;
	csElapsedTimeCb.Constants.Num32BitValues =	2;
	csElapsedTimeCb.ShaderVisibility =			D3D12_SHADER_VISIBILITY_ALL;

	// Fill out the root signature layout description.
	ZeroMemory(&rootSignatureDesc, sizeof(rootSignatureDesc));
	rootSignatureDesc.NumParameters =		1;
	rootSignatureDesc.pParameters =			&matrixBufferDesc;
	rootSignatureDesc.NumStaticSamplers =	0;
	rootSignatureDesc.pStaticSamplers =		nullptr;
	rootSignatureDesc.Flags =				D3D12_ROOT_SIGNATURE_FLAG_NONE;

	// Serialize the signature, preparing it for creation on the device.
	THROW_IF_FAILED(
		D3D12SerializeRootSignature(
			&rootSignatureDesc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&signature,
			nullptr),
		L"Unable to serialize the root signature for initialization on the graphics device.",
		L"Compute Pipeline Failure"
	);

	// Create the root signature on our device.
	THROW_IF_FAILED(
		device->CreateRootSignature(
			0,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(&m_rootSignature)),
		L"Unable to create the root signature for this graphics pipeline.",
		L"Compute Pipeline Failure"
	);
}


void ParticleComputeClass::SetShaderBytecode()
{
	// Create the descriptor for the vertex shader bytecode.
	m_csBytecode.pShaderBytecode =	g_particlecs;
	m_csBytecode.BytecodeLength =	sizeof(g_particlecs);
}


void ParticleComputeClass::NameD3D12Resources()
{
	std::wstring name;


	// Name all DirectX objects.
	m_rootSignature->SetName(L"PCC root signature");
	m_pipelineState->SetName(L"PCC pipeline state");
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		name = std::wstring(L"PCC command allocator ") + std::to_wstring(i);
		m_commandAllocators[i]->SetName(name.c_str());
	}
	m_commandList->SetName(L"PPC graphics command list");
	m_constantBuffer->SetName(L"PPC matrix buffer");
}
