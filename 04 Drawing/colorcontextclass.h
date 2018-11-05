////////////////////////////////////////////////////////////////////////////////
// Filename: colorcontextclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "rendercontextinterface.h"


/////////////
// SHADERS //
/////////////
#include "color.vs.h"
#include "color.ps.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ColorContextClass
////////////////////////////////////////////////////////////////////////////////
class ColorContextClass : public RenderContextInterface
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	ColorContextClass() = delete;
	ColorContextClass(const ColorContextClass&) = delete;
	ColorContextClass& operator=(const ColorContextClass&) = delete;

	ColorContextClass(ID3D12Device*, UINT, UINT, float, float);
	~ColorContextClass() = default;

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

	XMMATRIX	m_worldMatrix = XMMatrixIdentity();
	XMMATRIX	m_orthoMatrix = XMMatrixIdentity();
};
