////////////////////////////////////////////////////////////////////////////////
// Filename: solopipelineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "pipelineinterface.h"
#include "color.vs.h"
#include "color.ps.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: SoloPipelineClass
////////////////////////////////////////////////////////////////////////////////
class SoloPipelineClass : public PipelineInterface
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	SoloPipelineClass();
	SoloPipelineClass(const SoloPipelineClass&);
	~SoloPipelineClass();

	bool BeginPipeline(unsigned int, XMMATRIX) override;
	bool EndPipeline() override;

protected:
	bool InitializeRootSignature(ID3D12Device*) override;
	void SetShaderBytecode() override;
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputLayoutDesc() override;

private:
	UINT			m_matrixBufferwidth;
	ID3D12Resource*	m_matrixBuffer;
};
