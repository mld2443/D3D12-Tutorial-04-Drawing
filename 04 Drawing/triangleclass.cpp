////////////////////////////////////////////////////////////////////////////////
// Filename: triangleclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "triangleclass.h"


TriangleClass::TriangleClass(ID3D12Device* device) : GeometryInterface()
{
	std::vector<VertexType> vertices;
	std::vector<UINT32> indices;


	// Create the new vertex array.
	vertices.resize(3);

	// Create the new index array.
	indices.resize(3);

	// Load the vertex array with data.
	vertices[0].position =	XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color =		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position =	XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color =		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position =	XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color =		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	// Initialize the vertex buffer.
	InitializeVertexBuffer(device, vertices);

	// Initialize the vertex and index buffers.
	InitializeIndexBuffer(device, indices);
}
