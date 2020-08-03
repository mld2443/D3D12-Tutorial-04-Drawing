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
public:
    ColorContextClass() = delete;
    ColorContextClass(const ColorContextClass &) = delete;
    ColorContextClass& operator=(const ColorContextClass &) = delete;

    ColorContextClass(ID3D12Device *, const UINT &, const XMMATRIX &, const XMMATRIX &, UINT, UINT);
    ~ColorContextClass() = default;

    void SetShaderParameters(ID3D12GraphicsCommandList *) override;

protected:
    void InitializeRootSignature(ID3D12Device *) override;
    void SetShaderBytecode() override;
    void SetInputLayoutDesc() override;

    void NameD3DResources() override;

private:
    ConstantBufferType m_matrixBuffer = {};

    XMMATRIX m_worldMatrix = XMMatrixIdentity();
};
