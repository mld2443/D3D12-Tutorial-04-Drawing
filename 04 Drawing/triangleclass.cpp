////////////////////////////////////////////////////////////////////////////////
// Filename: triangleclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "triangleclass.h"


TriangleClass::TriangleClass(ID3D12Device* device): GeometryInterface()
{
	vector<VertexType> vertices;
	vector<UINT32> indices;


	// Create the new vertex array.
	vertices.resize(6);

	// Create the new index array.
	indices.resize(6);

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[3].position = XMFLOAT3(0.0f, 0.0f, 2.0f);  // Bottom left.
	vertices[3].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[4].position = XMFLOAT3(1.0f, 2.0f, 2.0f);  // Top middle.
	vertices[4].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[5].position = XMFLOAT3(2.0f, 0.0f, 2.0f);  // Bottom right.
	vertices[5].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	indices[3] = 3;  // Bottom left.
	indices[4] = 4;  // Top middle.
	indices[5] = 5;  // Bottom right.

	// Initialize the vertex buffer.
	m_vertexBuffer = BufferType(
		device,
		reinterpret_cast<BYTE*>(vertices.data()),
		vertices.size(),
		sizeof(VertexType),
		L"TC vertex buffer"
	);

	// Initialize the vertex and index buffers.
	m_indexBuffer = BufferType(
		device,
		reinterpret_cast<BYTE*>(indices.data()),
		indices.size(),
		sizeof(UINT32),
		DXGI_FORMAT_R32_UINT,
		L"TC index buffer"
	);
}


TriangleClass::~TriangleClass()
{
	SAFE_RELEASE(m_indexBuffer.buffer);
	SAFE_RELEASE(m_vertexBuffer.buffer);
}


void TriangleClass::Render(ID3D12GraphicsCommandList* commandList)
{
	// Set the type of primitive that the input assembler will try to assemble next.
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the vertex and index buffers as active in the input assembler so they will be used for rendering.
	commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer.vertexView);
	commandList->IASetIndexBuffer(&m_indexBuffer.indexView);

	// Issue the draw call for this geometry.
	commandList->DrawIndexedInstanced(static_cast<UINT>(m_indexBuffer.count), 1, 0, 0, 0);
}
