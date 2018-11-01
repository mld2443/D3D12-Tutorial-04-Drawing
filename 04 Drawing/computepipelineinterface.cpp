////////////////////////////////////////////////////////////////////////////////
// Filename: computepipelineinterface.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "computepipelineinterface.h"


ComputePipelineInterface::ComputePipelineInterface(ID3D12Device* device):
	PipelineInterface(device, D3D12_COMMAND_LIST_TYPE_COMPUTE)
{
}


void ComputePipelineInterface::InitializePipeline(ID3D12Device* device)
{
	// Check that the root signature is properly set up before using.
	THROW_IF_FALSE(
		m_rootSignature,
		L"Pipeline failed to initialize root signature correctly.",
		L"Graphics Pipeline Initialization Failure"
	);

	// First we need to set the sole description for the pipeline.
	SetShaderBytecode();

	// Then we can initialize the pipeline state and parameters.
	InitializeStateObject(device);
}


void ComputePipelineInterface::InitializeStateObject(ID3D12Device* device)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc;


	// Set up the Pipeline State for this compute pipeline.
	ZeroMemory(&pipelineStateDesc, sizeof(pipelineStateDesc));
	pipelineStateDesc.pRootSignature =	m_rootSignature;
	pipelineStateDesc.CS =				m_csBytecode;
	pipelineStateDesc.NodeMask =		0;
	pipelineStateDesc.Flags =			D3D12_PIPELINE_STATE_FLAG_NONE;

	// Create the pipeline state.
	THROW_IF_FAILED(
		device->CreateComputePipelineState(
			&pipelineStateDesc,
			IID_PPV_ARGS(&m_pipelineState)),
		L"The compute pipeline state object failed to initialize.",
		L"Compute Pipeline Initializer Failure"
	);
}


void ComputePipelineInterface::UpdateConstantBuffer(UINT frameIndex, BYTE* data, SIZE_T dataSize)
{
	D3D12_GPU_VIRTUAL_ADDRESS cbvAddress;


	// Declare the root signature.
	m_commandList->SetComputeRootSignature(m_rootSignature);

	// Set the data and get the address of the constant buffer for this frame.
	cbvAddress = SetConstantBuffer(frameIndex, data, dataSize);

	// Tell the root descriptor where the data for our matrix buffer is located.
	m_commandList->SetComputeRootConstantBufferView(0, cbvAddress);
}
