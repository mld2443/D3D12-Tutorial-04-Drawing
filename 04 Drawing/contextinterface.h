////////////////////////////////////////////////////////////////////////////////
// Filename: contextinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "pipelineclass.h"


////////////////////////////////////////////////////////////////////////////////
// Interface name: ContextInterface
////////////////////////////////////////////////////////////////////////////////
class ContextInterface
{
protected:
	struct ConstantBufferType
	{
		ComPtr<ID3D12Resource>	buffer =	nullptr;
		const SIZE_T			size =		0;
		const SIZE_T			stride =	0;

		ConstantBufferType() = default;
		ConstantBufferType(ID3D12Device*, SIZE_T);

		D3D12_GPU_VIRTUAL_ADDRESS SetConstantBuffer(UINT, BYTE*);
	};

public:
	ContextInterface() = delete;
	ContextInterface(const ContextInterface&) = delete;
	ContextInterface& operator=(const ContextInterface&) = delete;

	ContextInterface(pipeline_func, UINT&);
	virtual ~ContextInterface() = default;

	//virtual void SetShaderParameters(ID3D12GraphicsCommandList*) = 0;

protected:
	virtual void InitializeRootSignature(ID3D12Device*) = 0;
	virtual void InitializeContext(ID3D12Device*) = 0;
	virtual void InitializeState(ID3D12Device*) = 0;

	virtual void SetShaderBytecode() = 0;

	virtual void NameD3DResources() = 0;

public:
	const pipeline_func SetState;
	const pipeline_func SetShaderParameters;

protected:
	UINT&	r_frameIndex;

	ComPtr<ID3D12RootSignature>	m_rootSignature =	nullptr;
	ComPtr<ID3D12PipelineState>	m_state =	nullptr;
};
