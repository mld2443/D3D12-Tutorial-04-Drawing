////////////////////////////////////////////////////////////////////////////////
// Filename: pipelineinterface.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "pipelineinterface.h"


PipelineInterface::~PipelineInterface()
{
	// Release reserved resources.
	SAFE_RELEASE(m_constantBuffer);
	SAFE_RELEASE(m_commandList);
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		SAFE_RELEASE(m_commandAllocators[i]);
	}
	SAFE_RELEASE(m_pipelineState);
	SAFE_RELEASE(m_rootSignature);
}


ID3D12CommandList* PipelineInterface::GetCommandList()
{
	return m_commandList;
}


void PipelineInterface::InitializeConstantBuffer(ID3D12Device* device)
{
	UINT64 bufferSize;
	D3D12_HEAP_PROPERTIES heapProps;
	D3D12_RESOURCE_DESC resourceDesc;


	// Because CPU and GPU are asynchronus, we need to make room for multiple frames worth of buffers.
	bufferSize = m_constantBufferWidth * FRAME_BUFFER_COUNT;

	// Create description for our constant buffer heap type.
	ZeroMemory(&heapProps, sizeof(heapProps));
	heapProps.Type =					D3D12_HEAP_TYPE_UPLOAD;
	heapProps.CPUPageProperty =			D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference =	D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask =		1;
	heapProps.VisibleNodeMask =			1;

	// Create a description for the memory resource itself.
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

	// Allocate the memory on the GPU.
	THROW_IF_FAILED(
		device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_constantBuffer)),
		L"Unable to allocate space on the graphics device.",
		L"Hardware Memory Allocation Failure"
	);
}


void PipelineInterface::InitializeCommandList(ID3D12Device* device)
{
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		// Create command allocators, one for each frame.
		THROW_IF_FAILED(
			device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&m_commandAllocators[i])),
			L"Unable to create the command allocator object.",
			L"Graphics Pipeline Initialization Failure"
		);
	}

	// Create a command list.  We use the first command allocator, since allocators get reset before use.
	THROW_IF_FAILED(
		device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocators[0],
			nullptr,
			IID_PPV_ARGS(&m_commandList)),
		L"Unable to create the command list object.",
		L"Graphics Pipeline Initialization Failure"
	);
}


D3D12_GPU_VIRTUAL_ADDRESS PipelineInterface::SetConstantBuffer(UINT frameIndex, BYTE* data, SIZE_T dataSize)
{
	SIZE_T bufferOffset;
	D3D12_RANGE range;
	BYTE* mappedResource;


	// Calculate the offset of the constant buffer for the current frame.
	bufferOffset = static_cast<SIZE_T>(frameIndex) * m_constantBufferWidth;

	// Create a zero-width read range, [0, 0].
	// This is a signal to the GPU to not worry about the accuracy of the data within.
	ZeroMemory(&range, sizeof(range));

	// Lock the constant buffer so it can be written to.
	THROW_IF_FAILED(
		m_constantBuffer->Map(0, &range, reinterpret_cast<void**>(&mappedResource)),
		L"Unable to access the memory of the graphics device.",
		L"Graphics Device Communication Failure"
	);

	// Copy the data to the buffer.
	memcpy(&mappedResource[bufferOffset], data, dataSize);
	//memcpy(mappedResource + bufferOffset, data, dataSize);

	// Set the range of data that we wrote to.
	range.Begin =	bufferOffset;
	range.End =		range.Begin + m_constantBufferWidth;

	// Unlock the constant buffer.
	m_constantBuffer->Unmap(0, &range);

	// Tell the root descriptor where the data for our matrix buffer is located.
	return m_constantBuffer->GetGPUVirtualAddress() + bufferOffset;
}
