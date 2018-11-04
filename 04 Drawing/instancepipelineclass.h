////////////////////////////////////////////////////////////////////////////////
// Filename: instancepipelineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "renderpipelineinterface.h"


/////////////
// SHADERS //
/////////////
#include "instance.vs.h"
#include "instance.ps.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: InstancePipelineClass
////////////////////////////////////////////////////////////////////////////////
class InstancePipelineClass : public RenderPipelineInterface
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	InstancePipelineClass() = delete;
	InstancePipelineClass(const InstancePipelineClass&) = delete;
	InstancePipelineClass& operator=(const InstancePipelineClass&) = delete;

	InstancePipelineClass(ID3D12Device*, UINT, UINT, float, float);
	~InstancePipelineClass() = default;

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
