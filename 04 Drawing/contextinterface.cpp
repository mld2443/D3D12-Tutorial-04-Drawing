////////////////////////////////////////////////////////////////////////////////
// Filename: contextinterface.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "contextinterface.h"


ContextInterface::ConstantBufferType::ConstantBufferType(ID3D12Device* device, SIZE_T size) :
	size(size),
	stride(BYTE_ALIGNED_WIDTH(size, 0xFFull))
{
	UINT64 bufferSize;
	D3D12_HEAP_PROPERTIES heapProps;
	D3D12_RESOURCE_DESC resourceDesc;


	// Because CPU and GPU are asynchronus, we need to make room for multiple frames worth of buffers.
	bufferSize = stride * FRAME_BUFFER_COUNT;

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
	resourceDesc.Alignment =			D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
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
			IID_PPV_ARGS(buffer.ReleaseAndGetAddressOf())),
		L"Unable to allocate space for the constant buffer on the graphics device.",
		L"Hardware Memory Allocation Failure"
	);
}


D3D12_GPU_VIRTUAL_ADDRESS ContextInterface::ConstantBufferType::SetConstantBuffer(UINT frameIndex, BYTE* data)
{
	SIZE_T bufferOffset;
	D3D12_RANGE range;
	BYTE* mappedResource;
	D3D12_GPU_VIRTUAL_ADDRESS address;


	// Calculate the offset of the constant buffer for the current frame.
	bufferOffset = static_cast<SIZE_T>(frameIndex) * stride;

	// Create a zero-width read range, [0, 0].
	// This is a signal to the GPU that we won't be reading the data within.
	ZeroMemory(&range, sizeof(range));

	// Lock the constant buffer so it can be written to.
	THROW_IF_FAILED(
		buffer->Map(0, &range, reinterpret_cast<void**>(&mappedResource)),
		L"Unable to access the constant buffer on the graphics device.",
		L"Graphics Device Communication Failure"
	);

	// Copy the data to the buffer.
	memcpy(mappedResource + bufferOffset, data, size);

	// Set the range of data that we wrote to.
	range.Begin =	bufferOffset;
	range.End =		bufferOffset + size;

	// Unlock the constant buffer.
	buffer->Unmap(0, &range);

	// Calculate the address of the data we just wrote to.
	address = buffer->GetGPUVirtualAddress() + bufferOffset;

	return address;
}


ContextInterface::ContextInterface(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
	// Create command allocators, one for each frame.
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		THROW_IF_FAILED(
			device->CreateCommandAllocator(
				type,
				IID_PPV_ARGS(m_commandAllocators[i].ReleaseAndGetAddressOf())),
			L"Unable to create the command allocator object.",
			L"Graphics Pipeline Initialization Failure"
		);
	}

	// Create a command list.  We use the first command allocator, since allocators get reset before use.
	THROW_IF_FAILED(
		device->CreateCommandList(
			0,
			type,
			m_commandAllocators[0].Get(),
			nullptr,
			IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())),
		L"Unable to create the command list object.",
		L"Graphics Pipeline Initialization Failure"
	);

	// Initially we need to close the command list during initialization as it is created in a recording state.
	ClosePipeline();
}


ID3D12GraphicsCommandList* ContextInterface::GetCommandList()
{
	return m_commandList.Get();
}


void ContextInterface::OpenPipeline(UINT frameIndex)
{
	// Reset the memory that was holding the previously submitted command list.
	THROW_IF_FAILED(
		m_commandAllocators[frameIndex]->Reset(),
		L"Unable to reset command allocator.  Its associated memory may still be in use.",
		L"Pipeline Access Error"
	);

	// Reset our command list to prepare it for new commands.
	THROW_IF_FAILED(
		m_commandList->Reset(
			m_commandAllocators[frameIndex].Get(),
			m_pipelineState.Get()),
		L"Unable to reset command list.  It may not have been closed or submitted properly.",
		L"Command List Reset Error"
	);
}


void ContextInterface::ClosePipeline()
{
	// Close the command list so it can be submitted to a command queue.
	THROW_IF_FAILED(
		m_commandList->Close(),
		L"Unable to close command list.  It may not have been reset properly.",
		L"Command List Close Error"
	);
}