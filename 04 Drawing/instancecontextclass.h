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
public:
    InstanceContextClass() = delete;
    InstanceContextClass(const InstanceContextClass &) = delete;
    InstanceContextClass& operator=(const InstanceContextClass &) = delete;

    InstanceContextClass(ID3D12Device *, const UINT &, const XMMATRIX &, const XMMATRIX &, UINT, UINT);
    ~InstanceContextClass() = default;

    void SetShaderParameters(ID3D12GraphicsCommandList *) override;

protected:
    void InitializeRootSignature(ID3D12Device *) override;
    void SetShaderBytecode() override;
    void SetInputLayoutDesc() override;

    void NameD3DResources() override;

private:
    ConstantBufferType m_matrixBuffer = ConstantBufferType();

    XMMATRIX m_worldMatrix = XMMatrixIdentity();
};
