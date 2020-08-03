////////////////////////////////////////////////////////////////////////////////
// Filename: colorcontextclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "colorcontextclass.h"


ColorContextClass::ColorContextClass(ID3D12Device   *device,
                                     const UINT     &frameIndex,
                                     const XMMATRIX &viewMatrix,
                                     const XMMATRIX &projectionMatrix,
                                     UINT            screenWidth,
                                     UINT            screenHeight)
    : RenderContextInterface(frameIndex,
                             viewMatrix,
                             projectionMatrix)
    , m_matrixBuffer(device, sizeof(MatrixBufferType))
{
    // We need to set up the root signature before creating the pipeline state object.
    InitializeRootSignature(device);

    // Then we can initialize the pipeline state and parameters.
    InitializeContext(device);

    // Initialize the viewport and scissor rectangle.
    InitializeViewport(screenWidth, screenHeight);

    // After all the resources are initialized, we will name all of our objects for graphics debugging.
    NameD3DResources();
}


void ColorContextClass::SetShaderParameters(ID3D12GraphicsCommandList *commandList)
{
    // Set the window viewport.
    commandList->RSSetViewports(1, &m_viewport);
    commandList->RSSetScissorRects(1, &m_scissorRect);

    // Declare the root signature.
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    // Transpose and copy the matrices into the constant buffer.
    MatrixBufferType matrices;
    matrices.world      = XMMatrixTranspose(m_worldMatrix);
    matrices.view       = XMMatrixTranspose(r_viewMatrix);
    matrices.projection = XMMatrixTranspose(r_projectionMatrix);

    // Set the data and get the address of the constant buffer for this frame.
    D3D12_GPU_VIRTUAL_ADDRESS cbvAddress = m_matrixBuffer.SetConstantBuffer(r_frameIndex, reinterpret_cast<BYTE*>(&matrices));

    // Tell the root descriptor where the data for our matrix buffer is located.
    commandList->SetGraphicsRootConstantBufferView(0, cbvAddress);
}


void ColorContextClass::InitializeRootSignature(ID3D12Device *device)
{
    // Create a descriptor for the matrix buffer.
    D3D12_ROOT_PARAMETER matrixBufferDesc{};
    matrixBufferDesc.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    matrixBufferDesc.Descriptor.ShaderRegister = 0;
    matrixBufferDesc.Descriptor.RegisterSpace  = 0;
    matrixBufferDesc.ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;

    // Specify which shaders need access to what resources.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags;
    rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
                       | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
                       | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
                       | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
                       | D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    // Fill out the root signature layout description.
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.NumParameters     = 1;
    rootSignatureDesc.pParameters       = &matrixBufferDesc;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers   = nullptr;
    rootSignatureDesc.Flags             = rootSignatureFlags;

    // Serialize the signature, preparing it for creation on the device.
    ComPtr<ID3D10Blob> signature;
    THROW_IF_FAILED(
        D3D12SerializeRootSignature(
            &rootSignatureDesc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            signature.ReleaseAndGetAddressOf(),
            nullptr),
        "Unable to serialize the root signature for initialization on the graphics device."
    );

    // Create the root signature on our device.
    THROW_IF_FAILED(
        device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf())),
        "Unable to create the root signature for this graphics pipeline."
    );
}


void ColorContextClass::SetShaderBytecode()
{
    // Create the descriptor for the vertex shader bytecode.
    m_vsBytecode.pShaderBytecode = g_colorvs;
    m_vsBytecode.BytecodeLength  = sizeof(g_colorvs);

    // Create the descriptor for the pixel shader bytecode.
    m_psBytecode.pShaderBytecode = g_colorps;
    m_psBytecode.BytecodeLength  = sizeof(g_colorps);
}


void ColorContextClass::SetInputLayoutDesc()
{
    // Set the size of the layout description.
    m_inputLayoutDesc.resize(2);

    // Create the vertex input layout description. This needs to match the VertexType
    // stucture in the geometry interface and the VertexInputType in the shader.
    m_inputLayoutDesc[0].SemanticName         = "POSITION";
    m_inputLayoutDesc[0].SemanticIndex        = 0;
    m_inputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
    m_inputLayoutDesc[0].InputSlot            = 0;
    m_inputLayoutDesc[0].AlignedByteOffset    = 0;
    m_inputLayoutDesc[0].InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    m_inputLayoutDesc[0].InstanceDataStepRate = 0;

    m_inputLayoutDesc[1].SemanticName         = "COLOR";
    m_inputLayoutDesc[1].SemanticIndex        = 0;
    m_inputLayoutDesc[1].Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
    m_inputLayoutDesc[1].InputSlot            = 0;
    m_inputLayoutDesc[1].AlignedByteOffset    = D3D12_APPEND_ALIGNED_ELEMENT;
    m_inputLayoutDesc[1].InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    m_inputLayoutDesc[1].InstanceDataStepRate = 0;
}


void ColorContextClass::NameD3DResources()
{
    // Name all DirectX objects.
    m_rootSignature->SetName(L"CCC root signature");
    m_state->SetName(L"CCC pipeline state");
    m_matrixBuffer.buffer->SetName(L"CCC matrix buffer");
}
