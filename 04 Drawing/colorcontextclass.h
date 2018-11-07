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

	ColorContextClass(ID3D12Device*, UINT&, XMMATRIX&, XMMATRIX&, UINT, UINT);
	~ColorContextClass() = default;

protected:
	void InitializeRootSignature(ID3D12Device*) override;
	void SetShaderBytecode() override;
	void SetInputLayoutDesc() override;

	void NameD3DResources() override;

private:
	XMMATRIX&	r_viewMatrix;
	XMMATRIX&	r_projectionMatrix;

	ConstantBufferType	m_matrixBuffer =	ConstantBufferType();

	XMMATRIX	m_worldMatrix = XMMatrixIdentity();
};
