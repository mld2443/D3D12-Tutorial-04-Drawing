////////////////////////////////////////////////////////////////////////////////
// Filename: colorpipelineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "renderpipelineinterface.h"


/////////////
// SHADERS //
/////////////
#include "color.vs.h"
#include "color.ps.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ColorPipelineClass
////////////////////////////////////////////////////////////////////////////////
class ColorPipelineClass : public RenderPipelineInterface
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	ColorPipelineClass() = delete;
	ColorPipelineClass(const ColorPipelineClass&) = delete;
	ColorPipelineClass& operator=(const ColorPipelineClass&) = delete;

	ColorPipelineClass(ID3D12Device*, UINT, UINT, float, float);
	~ColorPipelineClass() = default;

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
