////////////////////////////////////////////////////////////////////////////////
// Filename: solopipelineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "pipelineinterface.h"


/////////////
// SHADERS //
/////////////
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
	SoloPipelineClass() = delete;
	SoloPipelineClass(const SoloPipelineClass&) = delete;
	SoloPipelineClass& operator=(const SoloPipelineClass&) = delete;

	SoloPipelineClass(ID3D12Device*, UINT, UINT, UINT, float, float);
	~SoloPipelineClass();

	void SetPipelineParameters(UINT, XMMATRIX, XMMATRIX) override;

protected:
	void InitializeRootSignature(ID3D12Device*);
	void SetShaderBytecode() override;
	void SetInputLayoutDesc() override;

private:
	void NameResources();

private:
	UINT			m_matrixBufferWidth;
	ID3D12Resource*	m_matrixBuffer;
};
