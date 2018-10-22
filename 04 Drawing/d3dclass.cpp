////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
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
	m_commandAllocator = nullptr;
	m_commandList = nullptr;
	m_pipelineState = nullptr;
	m_fence = nullptr;
	m_fenceEvent = nullptr;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}


bool D3DClass::Initialize(int screenHeight, int screenWidth, HWND hwnd, bool vsync, bool fullscreen)
{
	HRESULT result;
	ID3D12Debug* debugController;
	D3D_FEATURE_LEVEL featureLevel;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	UINT dxgiFlags;
	IDXGIFactory4* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, renderTargetViewDescriptorSize;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	IDXGISwapChain* swapChain;
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;


	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Set the feature level to DirectX 12.1 to enable using all the DirectX 12 features.
	// Note: Not all cards support full DirectX 12, this feature level may need to be reduced on some cards to 12.0.
	featureLevel = D3D_FEATURE_LEVEL_12_1;

	// Set the default DXGI factory flags.
	dxgiFlags = 0;

#if defined(_DEBUG)
	// Create the Direct3D debug controller.
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not enable Direct3D debugging.", L"Debugger Failure", MB_OK);
		return false;
	}

	// Enable the debug layer.
	debugController->EnableDebugLayer();

	// Release the debug controller.
	debugController->Release();
	debugController = nullptr;

	// Enable debugging our DXGI device.
	dxgiFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	// Create the Direct3D 12 device.
	result = D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_device));
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not create a DirectX 12.1 device.  The default video card does not support DirectX 12.1.", L"DirectX Device Failure", MB_OK);
		return false;
	}

	// Initialize the description of the command queue.
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));

	// Set up the description of the command queue.
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
	for (i = 0; i < numModes; i++)
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

	// Finally get the initial index to which buffer is the current back buffer.
	m_bufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Create a command allocator.
	result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
	if (FAILED(result))
	{
		return false;
	}

	// Create a basic command list.
	result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator, nullptr, IID_PPV_ARGS(&m_commandList));
	if (FAILED(result))
	{
		return false;
	}

	// Initially we need to close the command list during initialization as it is created in a recording state.
	result = m_commandList->Close();
	if (FAILED(result))
	{
		return false;
	}

	// Create a fence for GPU synchronization.
	result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	if (FAILED(result))
	{
		return false;
	}

	// Create an event object for the fence.
	m_fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_fenceEvent == NULL)
	{
		return false;
	}

	// Initialize the starting fence value. 
	m_fenceValue = 1;

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

	// Release the fence.
	if (m_fence)
	{
		m_fence->Release();
		m_fence = nullptr;
	}

	// Release the empty pipe line state.
	if (m_pipelineState)
	{
		m_pipelineState->Release();
		m_pipelineState = nullptr;
	}

	// Release the command list.
	if (m_commandList)
	{
		m_commandList->Release();
		m_commandList = nullptr;
	}

	// Release the command allocator.
	if (m_commandAllocator)
	{
		m_commandAllocator->Release();
		m_commandAllocator = nullptr;
	}

	// Release the back buffer render target views.
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		if (m_backBufferRenderTarget[i])
		{
			m_backBufferRenderTarget[i]->Release();
			m_backBufferRenderTarget[i] = nullptr;
		}
	}

	// Release the render target view heap.
	if (m_renderTargetViewHeap)
	{
		m_renderTargetViewHeap->Release();
		m_renderTargetViewHeap = nullptr;
	}

	// Release the swap chain.
	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}

	// Release the command queue.
	if (m_commandQueue)
	{
		m_commandQueue->Release();
		m_commandQueue = nullptr;
	}

	// Release the device.
	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	return;
}


bool D3DClass::Render(float red, float green, float blue, float alpha)
{
	HRESULT result;
	D3D12_RESOURCE_BARRIER barrier;
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
	unsigned int renderTargetViewDescriptorSize;
	float color[4];
	ID3D12CommandList* ppCommandLists[1];
	unsigned long long fenceToWaitFor;


	// Reset (re-use) the memory associated command allocator.
	result = m_commandAllocator->Reset();
	if (FAILED(result))
	{
		return false;
	}

	// Reset the command list, use empty pipeline state for now since there are no shaders and we are just clearing the screen.
	result = m_commandList->Reset(m_commandAllocator, m_pipelineState);
	if (FAILED(result))
	{
		return false;
	}

	// Record commands in the command list now.
	// Start by setting the resource barrier.
	barrier.Flags =						D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource =		m_backBufferRenderTarget[m_bufferIndex];
	barrier.Transition.StateBefore =	D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter =		D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource =	D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type =						D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	m_commandList->ResourceBarrier(1, &barrier);

	// Get the render target view handle for the current back buffer.
	renderTargetViewHandle = m_renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
	renderTargetViewDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	renderTargetViewHandle.ptr += renderTargetViewDescriptorSize * m_bufferIndex;

	// Set the back buffer as the render target.
	m_commandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, nullptr);

	// Then set the color to clear the window to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;
	m_commandList->ClearRenderTargetView(renderTargetViewHandle, color, 0, nullptr);

	// Indicate that the back buffer will now be used to present.
	barrier.Transition.StateBefore =	D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter =		D3D12_RESOURCE_STATE_PRESENT;
	m_commandList->ResourceBarrier(1, &barrier);

	// Close the list of commands.
	result = m_commandList->Close();
	if (FAILED(result))
	{
		return false;
	}

	// Load the command list array (only one command list for now).
	ppCommandLists[0] = m_commandList;

	// Execute the list of commands.
	m_commandQueue->ExecuteCommandLists(1, ppCommandLists);

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

	// Signal and increment the fence value.
	fenceToWaitFor = m_fenceValue;
	result = m_commandQueue->Signal(m_fence, fenceToWaitFor);
	if (FAILED(result))
	{
		return false;
	}
	m_fenceValue++;

	// Wait until the GPU is done rendering.
	if (m_fence->GetCompletedValue() < fenceToWaitFor)
	{
		result = m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent);
		if (FAILED(result))
		{
			return false;
		}
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	// Advance the back buffer index to the next frame.
	++m_bufferIndex;
	m_bufferIndex %= FRAME_BUFFER_COUNT;

	return true;
}