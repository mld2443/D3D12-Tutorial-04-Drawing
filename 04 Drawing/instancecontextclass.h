////////////////////////////////////////////////////////////////////////////////
// Filename: instancecontextclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "rendercontextinterface.h"


/////////////
// SHADERS //
/////////////
#include "instance.vs.h"
#include "instance.ps.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: InstanceContextClass
////////////////////////////////////////////////////////////////////////////////
class InstanceContextClass : public RenderContextInterface
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	InstanceContextClass() = delete;
	InstanceContextClass(const InstanceContextClass&) = delete;
	InstanceContextClass& operator=(const InstanceContextClass&) = delete;

	InstanceContextClass(ID3D12Device*, UINT, UINT, float, float);
	~InstanceContextClass() = default;

	XMMATRIX GetWorldMatrix();
	XMMATRIX GetOrthoMatrix();

	void SetPipelineParameters(UINT, XMMATRIX, XMMATRIX);

protected:
	void InitializeRootSignature(ID3D12Device*) override;
	void SetShaderBytecode() override;
	void SetInputLayoutDesc() override;

	void NameD3DResources() override;

private:
	ConstantBufferType	m_matrixBuffer =	ConstantBufferType();

	XMMATRIX	m_worldMatrix =	XMMatrixIdentity();
	XMMATRIX	m_orthoMatrix =	XMMatrixIdentity();
};
