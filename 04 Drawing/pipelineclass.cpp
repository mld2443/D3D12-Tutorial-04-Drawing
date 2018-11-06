////////////////////////////////////////////////////////////////////////////////
// Filename: pipelineclass.h
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "pipelineclass.h"


PipelineClass::PipelineClass(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
	// Create command allocators, one for each frame.
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		THROW_IF_FAILED(
			device->CreateCommandAllocator(
				type,
				IID_PPV_ARGS(m_commandAllocators[i].ReleaseAndGetAddressOf())),
			"Unable to create the command allocator object."
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
		"Unable to create the command list object."
	);

	// Initially we need to close the command list during initialization as it is created in a recording state.
	Close();
}


ID3D12GraphicsCommandList* PipelineClass::GetCommandList()
{
	return m_commandList.Get();
}


void PipelineClass::Open(UINT frameIndex)
{
	// Reset the memory that was holding the previously submitted command list.
	THROW_IF_FAILED(
		m_commandAllocators[frameIndex]->Reset(),
		"Unable to reset command allocator.  Its associated memory may still be in use."
	);

	// Reset our command list to prepare it for new commands.
	THROW_IF_FAILED(
		m_commandList->Reset(m_commandAllocators[frameIndex].Get(), nullptr),
		"Unable to reset command list.  It may not have been closed or submitted properly."
	);
}


void PipelineClass::Close()
{
	// Close the command list so it can be submitted to a command queue.
	THROW_IF_FAILED(
		m_commandList->Close(),
		"Unable to close command list.  It may not have been reset properly."
	);
}


PipelineClass& operator<<(PipelineClass& pipeline, PipelineClass::CommandType command)
{
	//
	switch (command)
	{
	case PipelineClass::open:
		break;
	case PipelineClass::end:
		// Close the command list so it can be submitted to a command queue.
		pipeline.Close();
		break;
	}

	return pipeline;
}


PipelineClass& operator<<(PipelineClass& pipeline, ID3D12PipelineState* state)
{
	// Set the pipeline state using the incoming PSO.
	pipeline.GetCommandList()->SetPipelineState(state);

	return pipeline;
}


PipelineClass& operator<<(PipelineClass& pipeline, D3D12_RESOURCE_BARRIER& barrier)
{
	// Put the barrier in the commandList.
	pipeline.GetCommandList()->ResourceBarrier(1, &barrier);

	return pipeline;
}
