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
	D3DClass(const D3DClass&) = delete;
	D3DClass& operator=(const D3DClass&) = delete;

	D3DClass(HWND, UINT, UINT, bool, bool);
	~D3DClass();

	ID3D12Device* GetDevice();
	UINT GetBufferIndex();

	void BeginScene(ID3D12GraphicsCommandList*, float, float, float, float);
	void EndScene(ID3D12GraphicsCommandList*);
	void SubmitToQueue(std::vector<ID3D12CommandList*>, bool);

	void WaitForNextAvailableFrame();
	void WaitForAllFrames();

private:
	void WaitForFrameIndex(UINT);

	void InitializeDevice();
	void InitializeCommandQueue();
	void InitializeSwapChain(HWND, UINT, UINT, bool, bool);
	void InitializeRenderTargets();
	void InitializeDepthStencil(UINT, UINT);
	void InitializeFences();

	void NameResources();

private:
	UINT	m_bufferIndex =		0;
	UINT	m_videoCardMemory =	0;

	IDXGISwapChain3*	m_swapChain =		nullptr;
	ID3D12Device*		m_device =			nullptr;
	ID3D12CommandQueue*	m_commandQueue =	nullptr;

	ID3D12DescriptorHeap*			m_renderTargetViewHeap =	nullptr;
	std::vector<ID3D12Resource*>	m_backBufferRenderTarget =	std::vector<ID3D12Resource*>(FRAME_BUFFER_COUNT, nullptr);
	ID3D12DescriptorHeap*			m_depthStencilViewHeap =	nullptr;
	ID3D12Resource*					m_depthStencil =			nullptr;

	std::vector<ID3D12Fence*>	m_fence =		std::vector<ID3D12Fence*>(FRAME_BUFFER_COUNT, nullptr);
	std::vector<UINT64>			m_fenceValue =	std::vector<UINT64>(FRAME_BUFFER_COUNT, 0);
	HANDLE						m_fenceEvent =	NULL;
};
