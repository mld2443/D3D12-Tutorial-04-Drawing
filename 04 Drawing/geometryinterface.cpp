////////////////////////////////////////////////////////////////////////////////
// Filename: geometryinterface.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "geometryinterface.h"


GeometryInterface::~GeometryInterface()
{
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);
}


UINT GeometryInterface::GetVertexCount()
{
	return m_vertexCount;
}


UINT GeometryInterface::GetIndexCount()
{
	return m_indexCount;
}


void GeometryInterface::Render(ID3D12GraphicsCommandList* commandList)
{
	// Set the type of primitive that the input assembler will try to assemble next.
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the vertex and index buffers as active in the input assembler so they will be used for rendering.
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->IASetIndexBuffer(&m_indexBufferView);

	// Issue the draw call for this geometry.
	commandList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);

	return;
}


void GeometryInterface::InitializeVertexBuffer(ID3D12Device* device,
											   const std::vector<VertexType>& vertices)
{
	// Set up default and upload heaps for the vertices, load the data and wait for completion.
	InitializeBuffer(
			device,
			&m_vertexBuffer,
			vertices,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			L"Vertex Buffer"
	);

	// Set the count of vertices.
	m_vertexCount = static_cast<UINT>(vertices.size());

	// Fill out the vertex buffer view for use while rendering.
	ZeroMemory(&m_vertexBufferView, sizeof(m_vertexBufferView));
	m_vertexBufferView.BufferLocation =	m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes =	static_cast<UINT>(sizeof(VertexType));
	m_vertexBufferView.SizeInBytes =	static_cast<UINT>(m_vertexBufferView.StrideInBytes * m_vertexCount);
}


void GeometryInterface::InitializeIndexBuffer(ID3D12Device* device,
											  const std::vector<UINT32>& indices)
{
	// Set up default and upload heaps for the indices, load the data and wait for completion.
	InitializeBuffer(
		device,
		&m_indexBuffer,
		indices,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		L"Index Buffer"
	);

	// Set the count of indices.
	m_indexCount = static_cast<UINT>(indices.size());

	// Fill out the index buffer view for use while rendering.
	ZeroMemory(&m_indexBufferView, sizeof(m_indexBufferView));
	m_indexBufferView.BufferLocation =	m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format =			DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes =		static_cast<UINT>(sizeof(UINT32) * m_indexCount);
}


template<typename BufferType>
void GeometryInterface::InitializeBuffer(ID3D12Device* device,
										 ID3D12Resource** buffer,
										 const std::vector<BufferType>& data,
										 D3D12_RESOURCE_STATES finalState,
										 std::wstring name)
{
	UINT bufferSize;
	D3D12_HEAP_PROPERTIES heapProps;
	D3D12_RESOURCE_DESC resourceDesc;
	ID3D12Resource* uploadBuffer;
	std::wstring uploadName;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList* commandList;
	D3D12_COMMAND_QUEUE_DESC queueDesc;
	ID3D12CommandQueue* commandQueue;
	BYTE* rawData;
	D3D12_RESOURCE_BARRIER barrierDesc;
	ID3D12CommandList* ppCommandLists[1];
	ID3D12Fence* fence;
	HANDLE fenceEvent;
	DWORD waitValue;


	// Calculate the size of the buffers.
	bufferSize = static_cast<UINT>(data.size() * sizeof(BufferType));

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
	resourceDesc.Width =				bufferSize;
	resourceDesc.Height =				1;
	resourceDesc.DepthOrArraySize =		1;
	resourceDesc.MipLevels =			1;
	resourceDesc.Format =				DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count =		1;
	resourceDesc.SampleDesc.Quality =	0;
	resourceDesc.Layout =				D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags =				D3D12_RESOURCE_FLAG_NONE;

	// Allocate space on the GPU for the defualt heap.
	THROW_IF_FAILED(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE, &resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(buffer)),
		L"Unable to allocate room on the device for the buffer heap.",
		L"Heap Allocation Failure"
	);

	// Set the name of the default heap for use in debugging.
	(*buffer)->SetName(name.c_str());

	// Change the heap type for the upload heap; this allows the CPU to write to it.
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// Allocate space on the GPU for the upload heap for this resource.
	THROW_IF_FAILED(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer)),
		L"Unable to allocate room on the device for the buffer upload heap.",
		L"Heap Allocation Failure"
	);

	// Set the name of the upload heap for use in debugging.
	uploadName = name + L" upload heap";
	uploadBuffer->SetName(uploadName.c_str());

	// Create a single-use command allocator.
	THROW_IF_FAILED(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator)),
		L"Unable to create Command Allocator on the device.",
		L"Command Allocator Creation Failure"
	);

	// Create a command list using our new command allocator.
	THROW_IF_FAILED(device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator,
			nullptr,
			IID_PPV_ARGS(&commandList)),
		L"Unable to create a command list on the device.",
		L"Command List Creation Failure"
	);

	// Fill out a description for a command queue.
	ZeroMemory(&queueDesc, sizeof(queueDesc));
	queueDesc.Type =		D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority =	D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags =		D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask =	0;

	// Create a single-use command queue.
	THROW_IF_FAILED(device->CreateCommandQueue(
			&queueDesc,
			IID_PPV_ARGS(&commandQueue)),
		L"Unable to create a command queue on the device.",
		L"Command Queue Creation Failure"
	);

	// Lock the upload buffer.
	THROW_IF_FAILED(uploadBuffer->Map(
			0,
			nullptr,
			reinterpret_cast<void**>(&rawData)),
		L"Unable to communicate with device buffer.",
		L"Memory Access Failure"
	);

	// Upload the data to our upload buffer.
	memcpy(rawData, data.data(), bufferSize);

	// Unlock the upload buffer.
	uploadBuffer->Unmap(0, nullptr);

	// Add the copy command to the command list.
	commandList->CopyBufferRegion(*buffer, 0, uploadBuffer, 0, bufferSize);

	// Fill out the description for our resource barrier.
	ZeroMemory(&barrierDesc, sizeof(barrierDesc));
	barrierDesc.Type =						D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Flags =						D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierDesc.Transition.pResource =		*buffer;
	barrierDesc.Transition.StateBefore =	D3D12_RESOURCE_STATE_COPY_DEST;
	barrierDesc.Transition.StateAfter =		finalState;
	barrierDesc.Transition.Subresource =	D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// Add the barrier to the command list.
	commandList->ResourceBarrier(1, &barrierDesc);

	// Close the command list.
	commandList->Close();

	// Prepare the list to be queued.
	ppCommandLists[0] = commandList;

	// Queue the list, starting the execution.
	commandQueue->ExecuteCommandLists(1, ppCommandLists);

	// Create our fence to track the commands.
	THROW_IF_FAILED(device->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&fence)),
		L"Unable to create fence to time the buffer upload.",
		L"Fence Creation Failure"
	);

	// Create an event that will block until our task is done.
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	THROW_IF_TRUE(fenceEvent == NULL,
		L"Unable to create system event for synchronization.",
		L"System Event Error"
	);

	// Tell the command queue what fence to use and the value we're waiting for.
	THROW_IF_FAILED(commandQueue->Signal(
			fence,
			1),
		L"Unable to signal to the fence when to stop execution.",
		L"Fence Communication Failure"
	);

	// Give our fence the event to use and the value to wait for.
	THROW_IF_FAILED(fence->SetEventOnCompletion(
			1,
			fenceEvent),
		L"Unable to set a fence event for synchronization.",
		L"Fence Communication failure"
	);

	// Wait up to 10 seconds for our task to complete or give up. This function is blocking.
	waitValue = WaitForSingleObject(fenceEvent, 10000);
	THROW_IF_FALSE(waitValue == WAIT_OBJECT_0,
		L"The Buffer took longer than 10 seconds to upload to the graphics device.",
		L"Buffer Upload Timed Out"
	);

	// Release temporary resources.
	rawData = nullptr;

	SAFE_RELEASE(uploadBuffer);
	SAFE_RELEASE(commandAllocator);
	SAFE_RELEASE(commandList);
	SAFE_RELEASE(commandQueue);
	SAFE_RELEASE(fence);
}
