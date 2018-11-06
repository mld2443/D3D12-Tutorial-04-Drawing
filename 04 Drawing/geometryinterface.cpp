////////////////////////////////////////////////////////////////////////////////
// Filename: geometryinterface.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "geometryinterface.h"


GeometryInterface::BufferType::BufferType(ID3D12Device* device, vector<UINT> data, wstring name) :
	count(data.size()),
	buffer(InitializeBuffer(
		device,
		reinterpret_cast<BYTE*>(data.data()),
		count * sizeof(UINT),
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		name
	)),
	indexView{
		buffer->GetGPUVirtualAddress(),
		static_cast<UINT>(count * sizeof(UINT)),
		DXGI_FORMAT_R32_UINT 
	}
{
}


ComPtr<ID3D12Resource> GeometryInterface::BufferType::InitializeBuffer(
	ID3D12Device* device,
	BYTE* data,
	SIZE_T dataSize,
	D3D12_RESOURCE_STATES finalState,
	wstring name)
{
	ComPtr<ID3D12Resource> defaultBuffer, uploadBuffer;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12Fence> fence;
	D3D12_HEAP_PROPERTIES heapProps;
	D3D12_RESOURCE_DESC resourceDesc;
	wstring uploadName;
	D3D12_COMMAND_QUEUE_DESC queueDesc;
	BYTE* rawData;
	D3D12_RESOURCE_BARRIER barrierDesc;
	ID3D12CommandList* ppCommandLists[1];
	HANDLE fenceEvent;
	DWORD waitValue;


	// Fill out a description for the default heap; the CPU cannot write to this heap.
	ZeroMemory(&heapProps, sizeof(heapProps));
	heapProps.Type =					D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty =			D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference =	D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask =		1;
	heapProps.VisibleNodeMask =			1;

	// Fill out a resource description for both heaps.
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension =			D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment =			0;
	resourceDesc.Width =				dataSize;
	resourceDesc.Height =				1;
	resourceDesc.DepthOrArraySize =		1;
	resourceDesc.MipLevels =			1;
	resourceDesc.Format =				DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count =		1;
	resourceDesc.SampleDesc.Quality =	0;
	resourceDesc.Layout =				D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags =				D3D12_RESOURCE_FLAG_NONE;

	// Allocate space on the GPU for the defualt heap.
	THROW_IF_FAILED(
		device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(defaultBuffer.ReleaseAndGetAddressOf())),
		"Unable to allocate room on the device for the buffer heap."
	);

	// Set the name of the default heap for use in debugging.
	defaultBuffer->SetName(name.c_str());

	// Change the heap type for the upload heap; this allows the CPU to write to it.
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// Allocate space on the GPU for the upload heap for this resource.
	THROW_IF_FAILED(
		device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf())),
		"Unable to allocate room on the device for the buffer upload heap."
	);

	// Set the name of the upload heap for use in debugging.
	uploadName = name + L" upload heap";
	uploadBuffer->SetName(uploadName.c_str());

	// Create a single-use command allocator.
	THROW_IF_FAILED(
		device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(commandAllocator.ReleaseAndGetAddressOf())),
		"Unable to create Command Allocator on the device."
	);

	// Create a command list using our new command allocator.
	THROW_IF_FAILED(
		device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(commandList.ReleaseAndGetAddressOf())),
		"Unable to create a command list on the device."
	);

	// Fill out a description for a command queue.
	ZeroMemory(&queueDesc, sizeof(queueDesc));
	queueDesc.Type =		D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority =	D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags =		D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask =	0;

	// Create a single-use command queue.
	THROW_IF_FAILED(
		device->CreateCommandQueue(
			&queueDesc,
			IID_PPV_ARGS(commandQueue.ReleaseAndGetAddressOf())),
		"Unable to create a command queue on the device."
	);

	// Lock the upload buffer.
	THROW_IF_FAILED(
		uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&rawData)),
		"Unable to communicate with device buffer."
	);

	// Upload the data to our upload buffer.
	memcpy(rawData, data, dataSize);

	// Unlock the upload buffer.
	uploadBuffer->Unmap(0, nullptr);

	// Add the copy command to the command list.
	commandList->CopyBufferRegion(defaultBuffer.Get(), 0, uploadBuffer.Get(), 0, dataSize);

	// Fill out the description for our resource barrier.
	ZeroMemory(&barrierDesc, sizeof(barrierDesc));
	barrierDesc.Type =						D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Flags =						D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierDesc.Transition.pResource =		defaultBuffer.Get();
	barrierDesc.Transition.StateBefore =	D3D12_RESOURCE_STATE_COPY_DEST;
	barrierDesc.Transition.StateAfter =		finalState;
	barrierDesc.Transition.Subresource =	D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// Add the barrier to the command list.
	commandList->ResourceBarrier(1, &barrierDesc);

	// Close the command list.
	commandList->Close();

	// Prepare the list to be queued.
	ppCommandLists[0] = commandList.Get();

	// Queue the list, starting the execution.
	commandQueue->ExecuteCommandLists(1, ppCommandLists);

	// Create our fence to track the commands.
	THROW_IF_FAILED(
		device->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(fence.ReleaseAndGetAddressOf())),
		"Unable to create fence to time the buffer upload."
	);

	// Create an event that will block until our task is done.
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	THROW_IF_TRUE(
		fenceEvent == NULL,
		"Unable to create system event for synchronization."
	);

	// Tell the command queue what fence to use and the value we're waiting for.
	THROW_IF_FAILED(
		commandQueue->Signal(fence.Get(), 1),
		"Unable to signal to the fence when to stop execution."
	);

	// Give our fence the event to use and the value to wait for.
	THROW_IF_FAILED(
		fence->SetEventOnCompletion(1, fenceEvent),
		"Unable to set a fence event for synchronization."
	);

	// Wait up to 10 seconds for our task to complete or give up. This function is blocking.
	waitValue = WaitForSingleObject(fenceEvent, 10000);
	THROW_IF_FALSE(
		waitValue == WAIT_OBJECT_0,
		"The Buffer took longer than 10 seconds to upload to the graphics device."
	);

	return defaultBuffer;
}
