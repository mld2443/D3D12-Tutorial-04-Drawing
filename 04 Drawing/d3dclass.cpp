////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "d3dclass.h"


D3DClass::D3DClass()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_commandQueue = nullptr;

	m_renderTargetViewHeap = nullptr;
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_backBufferRenderTarget[i] = nullptr;
	}
	m_depthStencilViewHeap = nullptr;
	m_depthStencil = nullptr;

	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_fence[i] = nullptr;
	}
	m_fenceEvent = nullptr;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}


bool D3DClass::Initialize(HWND hwnd, int screenHeight, int screenWidth, bool vsync, bool fullscreen)
{
	bool result;


	// Save the vsync setting.
	m_vsync_enabled = vsync;

	// Create the direct3D device.
	result = InitializeDevice(hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not create a DirectX 12.1 device.  The default video card does not support DirectX 12.1.", L"DirectX Device Failure", MB_OK);
		return false;
	}

	// Create the command queue where we submit all command lists.
	result = InitializeCommandQueue();
	if (!result)
	{
		return false;
	}

	// Create the swap chain that houses the back buffer.
	result = InitializeSwapChain(hwnd, screenWidth, screenHeight, fullscreen);
	if (!result)
	{
		return false;
	}

	// Create the render targets for drawing onto.
	result = InitializeRenderTargets();
	if (!result)
	{
		return false;
	}

	// Create the depth stencil buffer and its descriptor heap.
	result = InitializeDepthStencil(screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	// Create the fences and their values for handling asynchronous events.
	result = InitializeFences();
	if (!result)
	{
		return false;
	}

	// Finally, name our resources.
	NameResources();

	return true;
}


void D3DClass::Shutdown()
{
	int error;


	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, nullptr);
	}

	// Close the object handle to the fence event.
	error = CloseHandle(m_fenceEvent);
	if (error == 0)
	{
	}

	// Release reserved resources.
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		SAFE_RELEASE(m_fence[i]);
		SAFE_RELEASE(m_backBufferRenderTarget[i]);
	}
	SAFE_RELEASE(m_depthStencil);
	SAFE_RELEASE(m_depthStencilViewHeap);
	SAFE_RELEASE(m_renderTargetViewHeap);
	SAFE_RELEASE(m_swapChain);
	SAFE_RELEASE(m_commandQueue);
	SAFE_RELEASE(m_device);

	return;
}


ID3D12Device* D3DClass::GetDevice()
{
	return m_device;
}


unsigned int D3DClass::GetBufferIndex()
{
	return m_bufferIndex;
}


void D3DClass::BeginScene(ID3D12GraphicsCommandList* commandList,
						  float red, float green, float blue, float alpha)
{
	D3D12_RESOURCE_BARRIER barrier;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle, depthStencilViewHandle;
	unsigned int renderTargetViewDescriptorSize;
	float color[4];


	// Record commands in the command list now.
	// Start by setting the resource barrier.
	ZeroMemory(&barrier, sizeof(barrier));
	barrier.Flags =						D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource =		m_backBufferRenderTarget[m_bufferIndex];
	barrier.Transition.StateBefore =	D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter =		D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource =	D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type =						D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	commandList->ResourceBarrier(1, &barrier);

	// Get the render target view handle for the current back buffer.
	renderTargetViewHandle = m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
	renderTargetViewDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	renderTargetViewHandle.ptr += renderTargetViewDescriptorSize * m_bufferIndex;

	// Get the depth stencil view handle.
	depthStencilViewHandle = m_depthStencilViewHeap->GetCPUDescriptorHandleForHeapStart();

	// Set the back buffer as the render target.
	commandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, nullptr);

	// Then set the color to clear the window to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;
	commandList->ClearRenderTargetView(renderTargetViewHandle, color, 0, nullptr);

	// Finally, clear the depth stencil.
	commandList->ClearDepthStencilView(depthStencilViewHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	return;
}


void D3DClass::EndScene(ID3D12GraphicsCommandList* commandList)
{
	D3D12_RESOURCE_BARRIER barrier;


	// Indicate that the back buffer will now be used to present.
	ZeroMemory(&barrier, sizeof(barrier));
	barrier.Flags =						D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource =		m_backBufferRenderTarget[m_bufferIndex];
	barrier.Transition.StateBefore =	D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter =		D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource =	D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type =						D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	commandList->ResourceBarrier(1, &barrier);

	return;
}


bool D3DClass::SubmitToQueue(std::vector<ID3D12CommandList*> lists)
{
	HRESULT result;


	// Execute the list of commands.
	m_commandQueue->ExecuteCommandLists(static_cast<UINT>(lists.size()), lists.data());

	// Signal and increment the fence value.
	result = m_commandQueue->Signal(m_fence[m_bufferIndex], m_fenceValue[m_bufferIndex]);
	if (FAILED(result))
	{
		return false;
	}

	// Finally present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		result = m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		result = m_swapChain->Present(0, 0);
	}
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


bool D3DClass::WaitForFrameIndex(unsigned int frameIndex)
{
	HRESULT result;


	// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
	if (m_fence[frameIndex]->GetCompletedValue() < m_fenceValue[frameIndex])
	{
		// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
		result = m_fence[frameIndex]->SetEventOnCompletion(m_fenceValue[frameIndex], m_fenceEvent);
		if (FAILED(result))
		{
			return false;
		}

		// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
		// has reached "fenceValue", we know the command queue has finished executing
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	// increment fenceValue for next frame
	m_fenceValue[frameIndex]++;

	return true;
}


bool D3DClass::WaitForPreviousFrame()
{
	bool result;


	// Update the buffer index.
	m_bufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Wait for the last frame at this index to finish if it hasn't already.
	result = WaitForFrameIndex(m_bufferIndex);
	if (!result)
	{
		return false;
	}

	return true;
}


void D3DClass::WaitOnAllFrames()
{
	// Finish all commands already submitted to the GPU.
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		WaitForFrameIndex(i);
	}

	return;
}


bool D3DClass::InitializeDevice(HWND hwnd)
{
	HRESULT result;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D12Debug* debugController;


	// Set the feature level to DirectX 12.1 to enable using all the DirectX 12 features.
	// Note: Not all cards support full DirectX 12, this feature level may need to be reduced on some cards to 12.0.
	featureLevel = D3D_FEATURE_LEVEL_12_1;

#if defined(_DEBUG)
	// Create the Direct3D debug controller.
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	if (FAILED(result))
	{
		return false;
	}

	// Enable the debug layer.
	debugController->EnableDebugLayer();

	// Release the debug controller.
	debugController->Release();
	debugController = nullptr;
#endif

	// Create the Direct3D 12 device.
	result = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_device));
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


bool D3DClass::InitializeCommandQueue()
{
	HRESULT result;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;


	// Set up the description of the command queue.
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));
	commandQueueDesc.Type =		D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority =	D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags =	D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask =	0;

	// Create the command queue.
	result = m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue));
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


bool D3DClass::InitializeSwapChain(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	HRESULT result;
	UINT dxgiFlags;
	IDXGIFactory4* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	IDXGISwapChain* swapChain;


	// Set the default DXGI factory flags.
	dxgiFlags = 0;

#if defined(_DEBUG)
	// Enable debugging our DXGI device.
	dxgiFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory2(dxgiFlags, IID_PPV_ARGS(&factory));
	if (FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen height and width.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Height == (unsigned int)screenHeight)
		{
			if (displayModeList[i].Width == (unsigned int)screenWidth)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = nullptr;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = nullptr;

	// Release the adapter.
	adapter->Release();
	adapter = nullptr;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set the swap chain to use double buffering.
	swapChainDesc.BufferCount =			FRAME_BUFFER_COUNT;

	// Set the height and width of the back buffers in the swap chain.
	swapChainDesc.BufferDesc.Height =	screenHeight;
	swapChainDesc.BufferDesc.Width =	screenWidth;

	// Set a regular 32-bit surface for the back buffers.
	swapChainDesc.BufferDesc.Format =	DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the usage of the back buffers to be render target outputs.
	swapChainDesc.BufferUsage =			DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the swap effect to discard the previous buffer contents after swapping.
	swapChainDesc.SwapEffect =			DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow =		hwnd;

	// Set to full screen or windowed mode.
	if (fullscreen)
	{
		swapChainDesc.Windowed =	false;
	}
	else
	{
		swapChainDesc.Windowed =	true;
	}

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator =	numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator =	denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator =	0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator =	1;
	}

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count =			1;
	swapChainDesc.SampleDesc.Quality =			0;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering =	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling =			DXGI_MODE_SCALING_UNSPECIFIED;

	// Don't set the advanced flags.
	swapChainDesc.Flags =						0;

	// Finally create the swap chain using the swap chain description.	
	result = factory->CreateSwapChain(m_commandQueue, &swapChainDesc, &swapChain);
	if (FAILED(result))
	{
		return false;
	}

	// Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_swapChain.
	// This will allow us to use the newer functionality such as getting the current back buffer index.
	result = swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain));
	if (FAILED(result))
	{
		return false;
	}

	// Clear pointer to original swap chain interface since we are using version 3 instead (m_swapChain).
	swapChain = nullptr;

	// Release the factory now that the swap chain has been created.
	factory->Release();
	factory = nullptr;

	return true;
}


bool D3DClass::InitializeRenderTargets()
{
	HRESULT result;
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
	UINT renderTargetViewDescriptorSize;


	// Initialize the render target view heap description for the two back buffers.
	ZeroMemory(&renderTargetViewHeapDesc, sizeof(renderTargetViewHeapDesc));

	// Set the number of descriptors to two for our two back buffers.  Also set the heap type to render target views.
	renderTargetViewHeapDesc.NumDescriptors =	FRAME_BUFFER_COUNT;
	renderTargetViewHeapDesc.Type =				D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetViewHeapDesc.Flags =			D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// Create the render target view heap for the back buffers.
	result = m_device->CreateDescriptorHeap(&renderTargetViewHeapDesc, IID_PPV_ARGS(&m_renderTargetViewHeap));
	if (FAILED(result))
	{
		return false;
	}

	// Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
	renderTargetViewHandle = m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();

	// Get the size of the memory location for the render target view descriptors.
	renderTargetViewDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		// Get a pointer to the next back buffer from the swap chain.
		result = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBufferRenderTarget[i]));
		if (FAILED(result))
		{
			return false;
		}

		// Create a render target view for this back buffer.
		m_device->CreateRenderTargetView(m_backBufferRenderTarget[i], nullptr, renderTargetViewHandle);

		// Increment the view handle to the next descriptor location in the render target view heap.
		renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
	}

	// Also get the initial index to which buffer is the current back buffer.
	m_bufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	return true;
}


bool D3DClass::InitializeDepthStencil(int screenWidth, int screenHeight)
{
	HRESULT result;
	D3D12_CLEAR_VALUE depthOptimizedClearValue;
	D3D12_HEAP_PROPERTIES heapProps;
	D3D12_RESOURCE_DESC resourceDesc;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;


	//TODO: Fill out these comments.

	//
	ZeroMemory(&depthOptimizedClearValue, sizeof(depthOptimizedClearValue));
	depthOptimizedClearValue.Format =				DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth =	1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil =	0;

	//
	ZeroMemory(&heapProps, sizeof(heapProps));
	heapProps.Type =					D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty =			D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference =	D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask =		1;
	heapProps.VisibleNodeMask =			1;

	//
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

	//
	result = m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, IID_PPV_ARGS(&m_depthStencil));
	if (FAILED(result))
	{
		return false;
	}

	//
	ZeroMemory(&heapDesc, sizeof(heapDesc));
	heapDesc.NumDescriptors =	1;
	heapDesc.Type =				D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	heapDesc.Flags =			D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	//
	m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_depthStencilViewHeap));
	if (FAILED(result))
	{
		return false;
	}

	//
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format =			DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension =	D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags =			D3D12_DSV_FLAG_NONE;

	//
	m_device->CreateDepthStencilView(m_depthStencil, &depthStencilViewDesc, m_depthStencilViewHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}


bool D3DClass::InitializeFences()
{
	HRESULT result;


	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		// Create fences for GPU synchronization.
		result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence[i]));
		if (FAILED(result))
		{
			return false;
		}

		// Initialize the starting fence values.
		m_fenceValue[i] = 0;
	}

	// Create an event object for the fences.
	m_fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_fenceEvent == NULL)
	{
		return false;
	}

	return true;
}


void D3DClass::NameResources()
{
	std::wstring name;


	// Name all DirectX objects.
	m_device->SetName(L"D3DC device");
	m_commandQueue->SetName(L"D3DC command queue");
	m_renderTargetViewHeap->SetName(L"D3DC render target view heap");
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		name = std::wstring(L"D3DC back buffer render target ") + std::to_wstring(i);
		m_backBufferRenderTarget[i]->SetName(name.c_str());
	}
	m_depthStencilViewHeap->SetName(L"D3DC depth stencil view heap");
	m_depthStencil->SetName(L"D3DC depth stencil");
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		name = std::wstring(L"D3DC fence ") + std::to_wstring(i);
		m_fence[i]->SetName(name.c_str());
	}

	return;
}
