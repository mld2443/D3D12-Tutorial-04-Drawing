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
public:
	SoloPipelineClass();
	SoloPipelineClass(const SoloPipelineClass&);
	~SoloPipelineClass();

	bool UpdatePipeline(unsigned int) override;
	ID3D12CommandList* ClosePipeline() override;

protected:
	bool InitializeRootSignature(ID3D12Device*) override;
	void SetShaderBytecode() override;
	std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayoutDesc() override;
};
