////////////////////////////////////////////////////////////////////////////////
// Filename: quadclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "quadclass.h"


QuadClass::QuadClass(ID3D12Device* device) :
	GeometryInterface(
[=](ID3D12GraphicsCommandList* commandList)
{
	D3D12_VERTEX_BUFFER_VIEW pViews[2];


	// Set the views associated with this geometry vertex buffers.
	pViews[0] = m_vertexBuffer.vertexView;
	pViews[1] = m_instanceBuffer.vertexView;

	// Set the vertex and index buffers as active in the input assembler so they will be used for rendering.
	commandList->IASetVertexBuffers(0, 2, pViews);
	commandList->IASetIndexBuffer(&m_indexBuffer.indexView);

	// Set the type of primitive that the input assembler will try to assemble next.
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Issue the draw call for this geometry.
	commandList->DrawIndexedInstanced(static_cast<UINT>(m_indexBuffer.count), static_cast<UINT>(m_instanceBuffer.count), 0, 0, 0);
})
{
	vector<VertexType> vertices;
	vector<UINT32> indices;
	vector<InstanceType> instances;


	// Set the sizes of our vectors.
	vertices.resize(4);
	indices.resize(6);
	instances.resize(4);

	// Load the vertex vector with points of a square.
	vertices[0].position =	XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top left.
	vertices[0].color =		XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

	vertices[1].position =	XMFLOAT3(1.0f, 1.0f, 0.0f);  // Top right.
	vertices[1].color =		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[2].position =	XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color =		XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

	vertices[3].position =	XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[3].color =		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Describe the two triangles that make up a square.
	indices[0] = 0;  // Top left.
	indices[1] = 1;  // Top right.
	indices[2] = 2;  // Bottom right.

	indices[3] = 0;  // Top left.
	indices[4] = 2;  // Bottom right.
	indices[5] = 3;  // Bottom left.

	// Set four distinct copies of this square.
	instances[0].position = XMFLOAT3(-2.0f, 2.0f, 0.0f);
	instances[0].hsv =		XMFLOAT3(0.25f, 1.0f, 1.0f);

	instances[1].position = XMFLOAT3(2.0f, 2.0f, 0.0f);
	instances[1].hsv =		XMFLOAT3(0.5f, 1.0f, 1.0f);

	instances[2].position = XMFLOAT3(2.0f, -2.0f, 0.0f);
	instances[2].hsv =		XMFLOAT3(0.75f, 1.0f, 1.0f);

	instances[3].position = XMFLOAT3(-2.0f, -2.0f, 0.0f);
	instances[3].hsv =		XMFLOAT3(1.0f, 1.0f, 1.0f);

	// Initialize the vertex buffer and its view.
	m_vertexBuffer = BufferType(device, vertices, L"QC vertex buffer");

	// Initialize the index buffer and its view.
	m_indexBuffer = BufferType(device, indices, L"QC index buffer");

	// Initialize the instance buffer and its view.
	m_instanceBuffer = BufferType(device, instances, L"QC instance buffer");
}
