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
    D3DClass() = delete;
    D3DClass(const D3DClass &) = delete;
    D3DClass & operator=(const D3DClass &) = delete;

    D3DClass(HWND, UINT, UINT, bool, bool);
    ~D3DClass();

    ID3D12Device * GetDevice();
    uint32_t & GetBufferIndex();

    void SetClearColor(float, float, float, float);

    void SubmitToQueue(std::vector<ID3D12CommandList *>, bool);

    void WaitForNextAvailableFrame();
    void WaitForAllFrames();

    void ResetViewsCallback(ID3D12GraphicsCommandList *);

    D3D12_RESOURCE_BARRIER StartBarrier();
    D3D12_RESOURCE_BARRIER FinishBarrier();

private:
    void WaitForFrameIndex(const uint32_t);

    void InitializeDevice();
    void InitializeCommandQueue();
    void InitializeSwapChain(HWND, UINT, UINT, bool, bool);
    void InitializeRenderTargets();
    void InitializeDepthStencil(UINT, UINT);
    void InitializeFences();

    void NameResources();

private:
    uint32_t m_bufferIndex     = 0u;
    size_t   m_videoCardMemory = 0ull;
    float    m_clearColor[4]   = { 0.0f, 0.0f, 0.0f, 1.0f };

    ComPtr<IDXGISwapChain3>    m_swapChain    = nullptr;
    ComPtr<ID3D12Device>       m_device       = nullptr;
    ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;

    ComPtr<ID3D12DescriptorHeap>                           m_renderTargetViewHeap   = nullptr;
    std::array<ComPtr<ID3D12Resource>, FRAME_BUFFER_COUNT> m_backBufferRenderTarget = {};
    ComPtr<ID3D12DescriptorHeap>                           m_depthStencilViewHeap   = nullptr;
    ComPtr<ID3D12Resource>                                 m_depthStencil           = nullptr;

    std::array<ComPtr<ID3D12Fence>, FRAME_BUFFER_COUNT> m_fence      = {};
    std::array<UINT64, FRAME_BUFFER_COUNT>              m_fenceValue = {};
    HANDLE                                              m_fenceEvent = nullptr;
};
