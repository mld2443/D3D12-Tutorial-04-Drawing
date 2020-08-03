////////////////////////////////////////////////////////////////////////////////
// Filename: rendercontextinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "contextinterface.h"


////////////////////////////////////////////////////////////////////////////////
// Interface name: RenderContextInterface
////////////////////////////////////////////////////////////////////////////////
class RenderContextInterface : public ContextInterface
{
protected:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

public:
    RenderContextInterface() = delete;
    RenderContextInterface(const RenderContextInterface &) = delete;
    RenderContextInterface & operator=(const RenderContextInterface &) = delete;

    RenderContextInterface(const UINT &, const XMMATRIX &, const XMMATRIX &);
    virtual ~RenderContextInterface() = default;

    virtual void SetShaderParameters(ID3D12GraphicsCommandList *) = 0;

protected:
    virtual void InitializeRootSignature(ID3D12Device *) = 0;
    void InitializeContext(ID3D12Device *) override;
    virtual void InitializeViewport(UINT, UINT);
    virtual void InitializeState(ID3D12Device *) override;

    virtual void SetShaderBytecode() = 0;
    virtual void SetBlendDesc();
    virtual void SetRasterDesc();
    virtual void SetDepthStencilDesc();
    virtual void SetInputLayoutDesc() = 0;

    virtual void NameD3DResources() = 0;

protected:
    const XMMATRIX &r_viewMatrix;
    const XMMATRIX &r_projectionMatrix;

    D3D12_SHADER_BYTECODE                 m_vsBytecode       = {};
    D3D12_SHADER_BYTECODE                 m_hsBytecode       = {};
    D3D12_SHADER_BYTECODE                 m_dsBytecode       = {};
    D3D12_SHADER_BYTECODE                 m_gsBytecode       = {};
    D3D12_SHADER_BYTECODE                 m_psBytecode       = {};
    D3D12_BLEND_DESC                      m_blendDesc        = {};
    D3D12_RASTERIZER_DESC                 m_rasterDesc       = {};
    D3D12_DEPTH_STENCIL_DESC              m_depthStencilDesc = {};
    std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayoutDesc  = {};

    D3D12_VIEWPORT m_viewport    = {};
    D3D12_RECT     m_scissorRect = {};
};
