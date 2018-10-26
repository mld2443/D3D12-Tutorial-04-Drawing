////////////////////////////////////////////////////////////////////////////////
// Filename: solopipelineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <string>


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

	bool Initialize(ID3D12Device*, HWND, unsigned int, int, int, float, float) override;
	void Shutdown() override;

	bool SetPipelineParameters(unsigned int, XMMATRIX) override;

protected:
	bool InitializeRootSignature(ID3D12Device*) override;
	void SetShaderBytecode() override;
	void SetInputLayoutDesc() override;

private:
	void NameResources();

private:
	UINT			m_matrixBufferWidth;
	ID3D12Resource*	m_matrixBuffer;
};
