////////////////////////////////////////////////////////////////////////////////
// Filename: triangleclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "triangleclass.h"


TriangleClass::TriangleClass() : GeometryInterface()
{
}


TriangleClass::TriangleClass(const TriangleClass& other) : GeometryInterface(other)
{
}


TriangleClass::~TriangleClass()
{
}


bool TriangleClass::Initialize(ID3D12Device* device)
{
	bool result;
	std::vector<VertexType> vertices;
	std::vector<unsigned long> indices;


	// Create the new vertex array.
	vertices.resize(6);

	// Create the new index array.
	indices.resize(6);

	//TODO: Remove second triangle when done fixing depth buffer.

	// Load the vertex array with data.
	vertices[0].position =	XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color =		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position =	XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color =		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position =	XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color =		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[3].position =	XMFLOAT3(0.0f, 0.0f, -0.1f);  // Bottom left.
	vertices[3].color =		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[4].position =	XMFLOAT3(1.0f, 2.0f, -0.1f);  // Top middle.
	vertices[4].color =		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[5].position =	XMFLOAT3(2.0f, 0.0f, -0.1f);  // Bottom right.
	vertices[5].color =		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	indices[3] = 3;  // Bottom left.
	indices[4] = 4;  // Top middle.
	indices[5] = 5;  // Bottom right.

	// Initialize the vertex buffer.
	result = InitializeVertexBuffer(device, vertices);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeIndexBuffer(device, indices);
	if (!result)
	{
		return false;
	}

	return true;
}


void TriangleClass::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}
