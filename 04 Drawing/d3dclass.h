////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <d3d12.h>
#include <dxgi1_4.h>


/////////////////
// DEFENITIONS //
/////////////////
#define FRAME_BUFFER_COUNT 2


////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, HWND, bool, bool);
	void Shutdown();

	bool Render(float, float, float, float);

	ID3D12Device* GetDevice();

private:
	bool				m_vsync_enabled;
	int					m_videoCardMemory;
	char				m_videoCardDescription[128];
	IDXGISwapChain3*	m_swapChain;

	ID3D12Device*				m_device;
	ID3D12CommandQueue*			m_commandQueue;
	ID3D12DescriptorHeap*		m_renderTargetViewHeap;
	ID3D12Resource*				m_backBufferRenderTarget[FRAME_BUFFER_COUNT];
	unsigned int				m_bufferIndex;
	ID3D12CommandAllocator*		m_commandAllocator;
	ID3D12GraphicsCommandList*	m_commandList;
	ID3D12PipelineState*		m_pipelineState;
	ID3D12Fence*				m_fence;
	HANDLE						m_fenceEvent;
	unsigned long long			m_fenceValue;
};
