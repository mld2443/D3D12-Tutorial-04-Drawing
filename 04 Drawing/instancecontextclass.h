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

	InstanceContextClass(ID3D12Device*, UINT&, XMMATRIX&, XMMATRIX&, UINT, UINT);
	~InstanceContextClass() = default;

protected:
	void InitializeRootSignature(ID3D12Device*) override;
	void SetShaderBytecode() override;
	void SetInputLayoutDesc() override;

	void NameD3DResources() override;

private:
	XMMATRIX&	r_viewMatrix;
	XMMATRIX&	r_projectionMatrix;

	ConstantBufferType	m_matrixBuffer =	ConstantBufferType();

	XMMATRIX	m_worldMatrix =	XMMatrixIdentity();
};
