////////////////////////////////////////////////////////////////////////////////
// Filename: solopipelineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "graphicspipelineinterface.h"


/////////////
// SHADERS //
/////////////
#include "color.vs.h"
#include "color.ps.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: SoloPipelineClass
////////////////////////////////////////////////////////////////////////////////
class SoloPipelineClass : public GraphicsPipelineInterface
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

	SoloPipelineClass(ID3D12Device*, UINT, UINT, float, float);
	~SoloPipelineClass() = default;

	XMMATRIX GetWorldMatrix();
	XMMATRIX GetOrthoMatrix();

	void SetPipelineParameters(UINT, XMMATRIX, XMMATRIX);

protected:
	void InitializeRootSignature(ID3D12Device*) override;
	void SetShaderBytecode() override;
	void SetInputLayoutDesc() override;

	void NameD3D12Resources() override;

private:
	XMMATRIX	m_worldMatrix = XMMatrixIdentity();
	XMMATRIX	m_orthoMatrix = XMMatrixIdentity();
};
