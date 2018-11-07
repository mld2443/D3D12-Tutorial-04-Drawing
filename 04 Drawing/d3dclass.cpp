////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "d3dclass.h"


D3DClass::D3DClass(HWND hwnd, UINT screenWidth, UINT screenHeight, bool fullscreen, bool vsync)
{
	// Initialize the device and all the resources we will need while rendering.
	InitializeDevice();
	InitializeCommandQueue();
	InitializeSwapChain(hwnd, screenWidth, screenHeight, fullscreen, vsync);
	InitializeRenderTargets();
	InitializeDepthStencil(screenWidth, screenHeight);
	InitializeFences();

	// Finally, name our resources.
	NameResources();
}


D3DClass::~D3DClass()
{
	// Before shutting down, set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, nullptr);
	}

	// Make sure there are no more commands in the command queue by checking all fences one last time.
	ShutdownAllFrames();

	// Close the object handle to the fence event.
	CloseHandle(m_fenceEvent);
}


ID3D12Device* D3DClass::GetDevice()
{
	return m_device.Get();
}


UINT& D3DClass::GetBufferIndex()
{
	return m_bufferIndex;
}


void D3DClass::BeginScene(
	ID3D12GraphicsCommandList* commandList,
	float red, float green, float blue, float alpha)
{
	D3D12_RESOURCE_BARRIER barrier;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle, depthStencilViewHandle;
	unsigned int renderTargetViewDescriptorSize;
	float color[4];


	// Start by setting the resource barrier.
	ZeroMemory(&barrier, sizeof(barrier));
	barrier.Flags =						D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource =		m_backBufferRenderTarget[m_bufferIndex].Get();
	barrier.Transition.StateBefore =	D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter =		D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource =	D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type =						D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	commandList->ResourceBarrier(1, &barrier);

	// Get the render target view handle for the current back buffer.
	renderTargetViewHandle = m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
	renderTargetViewDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	renderTargetViewHandle.ptr += renderTargetViewDescriptorSize * static_cast<SIZE_T>(m_bufferIndex);

	// Get the depth stencil view handle.
	depthStencilViewHandle = m_depthStencilViewHeap->GetCPUDescriptorHandleForHeapStart();

	// Set the back buffer as the render target.
	commandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, &depthStencilViewHandle);

	// Then set the color to clear the window to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;
	commandList->ClearRenderTargetView(renderTargetViewHandle, color, 0, nullptr);

	// Finally, clear the depth stencil.
	commandList->ClearDepthStencilView(depthStencilViewHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}


void D3DClass::EndScene(ID3D12GraphicsCommandList* commandList)
{
	D3D12_RESOURCE_BARRIER barrier;


	// Indicate that the back buffer will now be used to present.
	ZeroMemory(&barrier, sizeof(barrier));
	barrier.Flags =						D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource =		m_backBufferRenderTarget[m_bufferIndex].Get();
	barrier.Transition.StateBefore =	D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter =		D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource =	D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type =						D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	commandList->ResourceBarrier(1, &barrier);
}


void D3DClass::SubmitToQueue(vector<ID3D12CommandList*> lists, bool vsync)
{
	// Execute the list of commands.
	m_commandQueue->ExecuteCommandLists(static_cast<UINT>(lists.size()), lists.data());

	// Put a command on the queue to signal this fence when it's done.
	THROW_IF_FAILED(
		m_commandQueue->Signal(
			m_fence[m_bufferIndex].Get(),
			m_fenceValue[m_bufferIndex]),
		"Unable to signal fence object."
	);

	// Finally present the back buffer to the screen since rendering is complete.
	THROW_IF_FAILED(
		m_swapChain->Present(vsync, 0),
		"Unable to present frame to the display."
	);
}


void D3DClass::WaitForNextAvailableFrame()
{
	// Update the buffer index.
	m_bufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Wait for the last frame at this index to finish if it hasn't already.
	WaitForFrameIndex(m_bufferIndex);

	// Increment fenceValue for the next frame.
	m_fenceValue[m_bufferIndex]++;
}


void D3DClass::ShutdownAllFrames()
{
	// Finish all commands already submitted to the GPU.
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		// Increment each fenceValue one final time.
		m_fenceValue[i]++;

		// Put a command on the queue to signal this fence when it's done.
		THROW_IF_FAILED(
			m_commandQueue->Signal(
				m_fence[i].Get(),
				m_fenceValue[i]),
			"Unable to signal fence object."
		);

		WaitForFrameIndex(i);
	}
}


void D3DClass::WaitForFrameIndex(UINT frameIndex)
{
	// If the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command.
	if (m_fence[frameIndex]->GetCompletedValue() < m_fenceValue[frameIndex])
	{
		// We have the fence signal the fenceEvent once the fence's current value is "fenceValue".
		THROW_IF_FAILED(
			m_fence[frameIndex]->SetEventOnCompletion(
				m_fenceValue[frameIndex],
				m_fenceEvent),
			"Unable to set the fence event."
		);

		// Wait for the fence event to complete, with no timeout.
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}


void D3DClass::InitializeDevice()
{
	D3D_FEATURE_LEVEL featureLevel;
	ComPtr<ID3D12Debug> debugController;


	// Set the feature level to DirectX 12.1 to enable using all the DirectX 12 features.
	featureLevel = D3D_FEATURE_LEVEL_12_1;

#if defined(_DEBUG)
	// Create the Direct3D 12 debug controller.
	THROW_IF_FAILED(
		D3D12GetDebugInterface(IID_PPV_ARGS(debugController.ReleaseAndGetAddressOf())),
		"Unable to create the Direct3D 12 debug layer."
	);

	// Enable the debug layer.
	debugController->EnableDebugLayer();
#endif // _DEBUG

	// Create the Direct3D 12 device.
	THROW_IF_FAILED(
		D3D12CreateDevice(
			nullptr,
			featureLevel,
			IID_PPV_ARGS(m_device.ReleaseAndGetAddressOf())),
		"Unable to create a DirectX 12.1 device.  The default video card does not support DirectX 12.1."
	);
}


void D3DClass::InitializeCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;


	// Set up the description of the command queue.
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));
	commandQueueDesc.Type =		D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority =	D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags =	D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask =	0;

	// Create the command queue.
	THROW_IF_FAILED(
		m_device->CreateCommandQueue(
			&commandQueueDesc,
			IID_PPV_ARGS(m_commandQueue.ReleaseAndGetAddressOf())),
		"Unable to create a command queue on the graphics device."
	);
}


void D3DClass::InitializeSwapChain(HWND hwnd, UINT screenWidth, UINT screenHeight, bool fullscreen, bool vsync)
{
	ComPtr<IDXGIFactory4> factory;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIOutput> adapterOutput;
	ComPtr<IDXGISwapChain> swapChain;
	UINT dxgiFlags;
	UINT numModes, numerator, denominator;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;


	// Set the default DXGI factory flags.
	dxgiFlags = 0;

#if defined(_DEBUG)
	// Enable debugging our DXGI device.
	dxgiFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

	// Create a DirectX graphics interface factory.
	THROW_IF_FAILED(
		CreateDXGIFactory2(
			dxgiFlags,
			IID_PPV_ARGS(factory.ReleaseAndGetAddressOf())),
		"Unable to create a device factory."
	);

	// Use the factory to create an adapter for the primary graphics interface (video card).
	THROW_IF_FAILED(
		factory->EnumAdapters(0, adapter.ReleaseAndGetAddressOf()),
		"Unable to enumerate adapters."
	);

	// Enumerate the primary adapter output (monitor).
	THROW_IF_FAILED(
		adapter->EnumOutputs(0, adapterOutput.ReleaseAndGetAddressOf()),
		"Unable to communicate with graphics adapter."
	);

	// Initialize numModes to zero.
	numModes = 0;

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	THROW_IF_FAILED(
		adapterOutput->GetDisplayModeList(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_ENUM_MODES_INTERLACED,
			&numModes,
			nullptr),
		"Unable to communicate with graphics adapter."
	);

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];

	// Now fill the display mode list structures.
	THROW_IF_FAILED(
		adapterOutput->GetDisplayModeList(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_ENUM_MODES_INTERLACED,
			&numModes,
			displayModeList),
		"Unable to communicate with graphics adapter."
	);

	// Initialize numerator and denominator to 0.
	numerator = denominator = 0;

	// Now go through all the display modes and find the one that matches the screen height and width.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (UINT i = 0; i < numModes; ++i)
	{
		if (displayModeList[i].Height == screenHeight && displayModeList[i].Width == screenWidth)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// Get the adapter (video card) description.
	THROW_IF_FAILED(
		adapter->GetDesc(&adapterDesc),
		"Unable to communicate with graphics adapter."
	);

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = static_cast<UINT>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = nullptr;

	// Create a description for the swap chain.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount =					FRAME_BUFFER_COUNT;
	swapChainDesc.BufferDesc.Height =			screenHeight;
	swapChainDesc.BufferDesc.Width =			screenWidth;
	swapChainDesc.BufferDesc.Format =			DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage =					DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect =					DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow =				hwnd;
	swapChainDesc.Windowed =					!fullscreen;
	swapChainDesc.SampleDesc.Count =			1;
	swapChainDesc.SampleDesc.Quality =			0;
	swapChainDesc.BufferDesc.ScanlineOrdering =	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling =			DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.Flags =						0;
	if (vsync)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator =	numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator =	denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator =	0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator =	1;
	}

	// Create the swap chain using the swap chain description.	
	THROW_IF_FAILED(
		factory->CreateSwapChain(
			m_commandQueue.Get(),
			&swapChainDesc,
			swapChain.ReleaseAndGetAddressOf()),
		"Unable to create the swap chain on the graphics device."
	);

	// Finally, upgrade the swap chain to a IDXGISwapChain3 interface and store it in the private member variable m_swapChain.
	// This will allow us to use the function GetCurrentBackBufferIndex().
	THROW_IF_FAILED(
		swapChain.As(&m_swapChain),
		"This graphics device does not support the 'IDXGISwapChain3' Interface."
	);
}


void D3DClass::InitializeRenderTargets()
{
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
	UINT renderTargetViewDescriptorSize;


	// Set the number of descriptors to two for our two back buffers.
	// Also set the heap type to render target views.
	ZeroMemory(&renderTargetViewHeapDesc, sizeof(renderTargetViewHeapDesc));
	renderTargetViewHeapDesc.NumDescriptors =	FRAME_BUFFER_COUNT;
	renderTargetViewHeapDesc.Type =				D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetViewHeapDesc.Flags =			D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// Create the render target view heap for the back buffers.
	THROW_IF_FAILED(
		m_device->CreateDescriptorHeap(
			&renderTargetViewHeapDesc,
			IID_PPV_ARGS(m_renderTargetViewHeap.ReleaseAndGetAddressOf())),
		"Unable to create the render target heap on the graphics device."
	);

	// Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
	renderTargetViewHandle = m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();

	// Get the size of the memory location for the render target view descriptors.
	renderTargetViewDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		// Get a pointer to the next back buffer from the swap chain.
		THROW_IF_FAILED(
			m_swapChain->GetBuffer(
				i,
				IID_PPV_ARGS(m_backBufferRenderTarget[i].ReleaseAndGetAddressOf())),
			"Unable to communicate with the swap chain."
		);

		// Create a render target view for this back buffer.
		m_device->CreateRenderTargetView(m_backBufferRenderTarget[i].Get(), nullptr, renderTargetViewHandle);

		// Increment the view handle to the next descriptor location in the render target view heap.
		renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
	}

	// Also get the initial index to which buffer is the current back buffer.
	m_bufferIndex = m_swapChain->GetCurrentBackBufferIndex();
}


void D3DClass::InitializeDepthStencil(UINT screenWidth, UINT screenHeight)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	D3D12_HEAP_PROPERTIES heapProps;
	D3D12_RESOURCE_DESC resourceDesc;
	D3D12_CLEAR_VALUE depthOptimizedClearValue;
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;


	// Define the heap where we'll keep the DSV.
	ZeroMemory(&heapDesc, sizeof(heapDesc));
	heapDesc.NumDescriptors =	1;
	heapDesc.Type =				D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	heapDesc.Flags =			D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// Create our heap.  It must be created before it can be used to create the DSV.
	THROW_IF_FAILED(
		m_device->CreateDescriptorHeap(
			&heapDesc,
			IID_PPV_ARGS(&m_depthStencilViewHeap)),
		"Unable to create the render target heap on the graphics device."
	);

	// Set the heap properties for the heap where we keep the DSV.  This heap
	// is inaccessible from the CPU, and everythin else is set to defaults.
	ZeroMemory(&heapProps, sizeof(heapProps));
	heapProps.Type =					D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty =			D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference =	D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask =		1;
	heapProps.VisibleNodeMask =			1;

	// Define the resource attributes of our DSV.  It is a 2D texture, with a resolution, and
	// the format is a D32 float.  This format should match our pipeline state object DSVformat.
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension =			D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment =			0;
	resourceDesc.Width =				screenWidth;
	resourceDesc.Height =				screenHeight;
	resourceDesc.DepthOrArraySize =		1;
	resourceDesc.MipLevels =			0;
	resourceDesc.Format =				DXGI_FORMAT_D32_FLOAT;
	resourceDesc.SampleDesc.Count =		1;
	resourceDesc.SampleDesc.Quality =	0;
	resourceDesc.Layout =				D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags =				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// Just like how the RTV needs a color to clear to, the DSV needs a value to
	// clear to.  We set this value as infimite depth, and black for the stencil.
	ZeroMemory(&depthOptimizedClearValue, sizeof(depthOptimizedClearValue));
	depthOptimizedClearValue.Format =				DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth =	1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil =	0;

	// Create the resource our depth stencil will be using.
	THROW_IF_FAILED(
		m_device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(&m_depthStencil)),
		"Unable to allocate the depth buffer on the graphics device."
	);

	// Define the attributes of our depth stencil.  These need to match what we
	// have used so far, as well as what is stated in the pipeline state object.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format =			DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension =	D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags =			D3D12_DSV_FLAG_NONE;

	// Finally, we can create the depth stencil view itself.
	m_device->CreateDepthStencilView(
		m_depthStencil.Get(),
		&depthStencilViewDesc,
		m_depthStencilViewHeap->GetCPUDescriptorHandleForHeapStart()
	);
}


void D3DClass::InitializeFences()
{
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		// Create fences for GPU synchronization.
		THROW_IF_FAILED(
			m_device->CreateFence(
				0,
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(m_fence[i].ReleaseAndGetAddressOf())),
			"Unable to create synchronization fences on the graphics device."
		);

		// Initialize the starting fence values.
		m_fenceValue[i] = 0;
	}

	// Create an event object for the fences.
	m_fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	THROW_IF_TRUE(
		m_fenceEvent == NULL,
		"Unable to create a windows system event for hardware synchronization."
	);
}


void D3DClass::NameResources()
{
	wstring name;


	// Name all DirectX objects.
	m_device->SetName(L"D3DC device");
	m_commandQueue->SetName(L"D3DC command queue");
	m_renderTargetViewHeap->SetName(L"D3DC render target view heap");
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		name = wstring(L"D3DC back buffer render target ") + to_wstring(i);
		m_backBufferRenderTarget[i]->SetName(name.c_str());
	}
	m_depthStencilViewHeap->SetName(L"D3DC depth stencil view heap");
	m_depthStencil->SetName(L"D3DC depth stencil");
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		name = wstring(L"D3DC fence ") + to_wstring(i);
		m_fence[i]->SetName(name.c_str());
	}
}
