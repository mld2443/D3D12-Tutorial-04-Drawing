////////////////////////////////////////////////////////////////////////////////
// Filename: triangleclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "geometryinterface.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TriangleClass
////////////////////////////////////////////////////////////////////////////////
class TriangleClass : public GeometryInterface
{
private:
	struct VertexType
	{
		XMFLOAT3 position =	XMFLOAT3();
		XMFLOAT4 color =	XMFLOAT4();
	};

public:
	TriangleClass() = delete;
	TriangleClass(const TriangleClass&) = delete;
	TriangleClass& operator=(const TriangleClass&) = delete;

	TriangleClass(ID3D12Device*);
	~TriangleClass() = default;

private:
	BufferType	m_vertexBuffer =	BufferType();
	BufferType	m_indexBuffer =		BufferType();
};
