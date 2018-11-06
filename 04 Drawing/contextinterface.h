////////////////////////////////////////////////////////////////////////////////
// Filename: contextinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


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
	ContextInterface(const ContextInterface&) = delete;
	ContextInterface& operator=(const ContextInterface&) = delete;

	ContextInterface() = default;
	~ContextInterface() = default;

	ID3D12PipelineState* GetPSO();

protected:
	virtual void InitializeRootSignature(ID3D12Device*) = 0;
	virtual void InitializeContext(ID3D12Device*) = 0;
	virtual void InitializeStateObject(ID3D12Device*) = 0;

	virtual void SetShaderBytecode() = 0;

	virtual void NameD3DResources() = 0;

protected:
	ComPtr<ID3D12RootSignature>	m_rootSignature =	nullptr;
	ComPtr<ID3D12PipelineState>	m_pipelineState =	nullptr;
};
