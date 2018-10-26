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
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	void Initialize(HWND, UINT, UINT, bool, bool);
	void Shutdown();

	ID3D12Device* GetDevice();
	UINT GetBufferIndex();

	void BeginScene(ID3D12GraphicsCommandList*, float, float, float, float);
	void EndScene(ID3D12GraphicsCommandList*);
	void SubmitToQueue(std::vector<ID3D12CommandList*>);

	void WaitForFrameIndex(unsigned int);
	void WaitForPreviousFrame();
	void WaitOnAllFrames();

private:
	void InitializeDevice();
	void InitializeCommandQueue();
	void InitializeSwapChain(HWND, UINT, UINT, bool);
	void InitializeRenderTargets();
	void InitializeDepthStencil(UINT, UINT);
	void InitializeFences();

	void NameResources();

private:
	bool	m_vsyncEnabled;
	UINT	m_bufferIndex;
	UINT	m_videoCardMemory;

	IDXGISwapChain3*	m_swapChain;
	ID3D12Device*		m_device;
	ID3D12CommandQueue*	m_commandQueue;

	ID3D12DescriptorHeap*	m_renderTargetViewHeap;
	ID3D12Resource*			m_backBufferRenderTarget[FRAME_BUFFER_COUNT];
	ID3D12DescriptorHeap*	m_depthStencilViewHeap;
	ID3D12Resource*			m_depthStencil;

	ID3D12Fence*	m_fence[FRAME_BUFFER_COUNT];
	HANDLE			m_fenceEvent;
	UINT64			m_fenceValue[FRAME_BUFFER_COUNT];
};
