////////////////////////////////////////////////////////////////////////////////
// Filename: contextinterface.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
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
		"Unable to allocate space for the constant buffer on the graphics device."
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
		"Unable to access the constant buffer on the graphics device."
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


ContextInterface::ContextInterface(pipeline_func setParameters, UINT& frameIndex) :
	r_frameIndex(frameIndex),
	SetShaderParameters(setParameters),
SetState([=](ID3D12GraphicsCommandList* commandList)
{
	commandList->SetPipelineState(m_state.Get());
})
{
}
