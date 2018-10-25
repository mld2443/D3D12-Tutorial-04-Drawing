////////////////////////////////////////////////////////////////////////////////
// Filename: solopipelineclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "solopipelineclass.h"


SoloPipelineClass::SoloPipelineClass()
{
}


SoloPipelineClass::SoloPipelineClass(const SoloPipelineClass& other): PipelineInterface(other)
{
}


SoloPipelineClass::~SoloPipelineClass()
{
}


bool SoloPipelineClass::UpdatePipeline(unsigned int frameIndex)
{
	HRESULT result;
	//ID3D12DescriptorHeap* descriptorHeaps[1];


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

	// Declare the root signature.
	m_commandList->SetGraphicsRootSignature(m_rootSignature);

	//TODO: implement these next lines.
	// Set constant buffer descriptor heap.
	//descriptorHeaps[0] = mainDescriptorHeap[frameIndex];
	//m_commandList->SetDescriptorHeaps(1, descriptorHeaps);

	// Set the root descriptor table 0 to the constant buffer descriptor heap.
	//m_commandList->SetGraphicsRootDescriptorTable(0, mainDescriptorHeap[frameIndex]->GetGPUDescriptorHandleForHeapStart());

	// Set the window viewport.
	m_commandList->RSSetViewports(1, &m_viewport);
	//m_commandList->RSSetScissorRects(1, &scissorRect);

	return true;
}


ID3D12CommandList* SoloPipelineClass::ClosePipeline()
{
	HRESULT result;


	// Close the command list so it can be submitted to a command queue.
	result = m_commandList->Close();
	if (FAILED(result))
	{
		return nullptr;
	}

	return m_commandList;
}


bool SoloPipelineClass::InitializeRootSignature(ID3D12Device* device)
{
	HRESULT result;
	D3D12_ROOT_PARAMETER matrixBufferDesc;
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	ID3D10Blob *signature;


	//TODO: Set up WVP matrices communication with the shader in this function.
	//TODO: decide if this CBV descriptor is necessary.

	//
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
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the root signature on our device.
	result = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	if (FAILED(result))
	{
		return false;
	}

	return true;
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

	return;
}


std::vector<D3D12_INPUT_ELEMENT_DESC>& SoloPipelineClass::GetInputLayoutDesc()
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> polygonLayout;


	// Set the size of the layout description.
	polygonLayout.resize(2);

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the geometry interface and in the shader.
	polygonLayout[0].SemanticName =			"POSITION";
	polygonLayout[0].SemanticIndex =		0;
	polygonLayout[0].Format =				DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot =			0;
	polygonLayout[0].AlignedByteOffset =	0;
	polygonLayout[0].InputSlotClass =		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate =	0;

	polygonLayout[1].SemanticName =			"COLOR";
	polygonLayout[1].SemanticIndex =		0;
	polygonLayout[1].Format =				DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot =			0;
	polygonLayout[1].AlignedByteOffset =	D3D12_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass =		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate =	0;

	return polygonLayout;
}
