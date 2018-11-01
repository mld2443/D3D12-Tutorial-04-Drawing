////////////////////////////////////////////////////////////////////////////////
// Filename: computepipelineinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "pipelineinterface.h"


////////////////////////////////////////////////////////////////////////////////
// Interface name: ComputePipelineInterface
////////////////////////////////////////////////////////////////////////////////
class ComputePipelineInterface : public PipelineInterface
{
public:
	ComputePipelineInterface() = delete;
	ComputePipelineInterface(const ComputePipelineInterface&) = delete;
	ComputePipelineInterface& operator=(const ComputePipelineInterface&) = delete;

	ComputePipelineInterface(ID3D12Device*);
	~ComputePipelineInterface() = default;

protected:
	virtual void InitializeRootSignature(ID3D12Device*) = 0;
	void InitializePipeline(ID3D12Device*) override;
	virtual void InitializeStateObject(ID3D12Device*) override;

	virtual void SetShaderBytecode() = 0;

	void UpdateConstantBuffer(UINT, BYTE*, SIZE_T);

	virtual void NameD3D12Resources() = 0;

protected:
	D3D12_SHADER_BYTECODE	m_csBytecode = D3D12_SHADER_BYTECODE();
};
