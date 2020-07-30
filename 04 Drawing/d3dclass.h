////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
    D3DClass(HWND, UINT, UINT, bool, bool);
    ~D3DClass();

    ID3D12Device* GetDevice();
    UINT& GetBufferIndex();

    void SetClearColor(float, float, float, float);

    void SubmitToQueue(vector<ID3D12CommandList*>, bool);

    void WaitForNextAvailableFrame();
    void WaitForAllFrames();

    void ResetViewsCallback(ID3D12GraphicsCommandList*);

    D3D12_RESOURCE_BARRIER StartBarrier();
    D3D12_RESOURCE_BARRIER FinishBarrier();

private:
    void WaitForFrameIndex(UINT);

    void InitializeDevice();
    void InitializeCommandQueue();
    void InitializeSwapChain(HWND, UINT, UINT, bool, bool);
    void InitializeRenderTargets();
    void InitializeDepthStencil(UINT, UINT);
    void InitializeFences();

    void NameResources();

public:
    const pipeline_func ResetViews;

private:
    UINT  m_bufferIndex     = 0;
    UINT  m_videoCardMemory = 0;
    float m_clearColor[4]   = { 0.0f, 0.0f, 0.0f, 1.0f };

    ComPtr<IDXGISwapChain3>    m_swapChain    = nullptr;
    ComPtr<ID3D12Device>       m_device       = nullptr;
    ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;

    ComPtr<ID3D12DescriptorHeap>   m_renderTargetViewHeap   = nullptr;
    vector<ComPtr<ID3D12Resource>> m_backBufferRenderTarget = vector<ComPtr<ID3D12Resource>>(FRAME_BUFFER_COUNT, nullptr);
    ComPtr<ID3D12DescriptorHeap>   m_depthStencilViewHeap   = nullptr;
    ComPtr<ID3D12Resource>         m_depthStencil           = nullptr;

    vector<ComPtr<ID3D12Fence>> m_fence      = vector<ComPtr<ID3D12Fence>>(FRAME_BUFFER_COUNT, nullptr);
    vector<UINT64>              m_fenceValue = vector<UINT64>(FRAME_BUFFER_COUNT, 0);
    HANDLE                      m_fenceEvent = nullptr;
};
