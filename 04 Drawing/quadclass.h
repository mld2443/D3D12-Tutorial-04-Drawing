////////////////////////////////////////////////////////////////////////////////
// Filename: quadclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "geometryinterface.h"
#include "pipelineinterface.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: QuadClass
////////////////////////////////////////////////////////////////////////////////
class QuadClass : public GeometryInterface
{
private:
	struct VertexType
	{
		XMFLOAT3 position =	XMFLOAT3();
		XMFLOAT4 color =	XMFLOAT4();
	};

	struct InstanceType
	{
		XMFLOAT3 position =	XMFLOAT3();
		XMFLOAT3 hsv =		XMFLOAT3();
	};

public:
	QuadClass() = delete;
	QuadClass(const QuadClass&) = delete;
	QuadClass& operator=(const QuadClass&) = delete;

	QuadClass(ID3D12Device*);
	~QuadClass() = default;

	void Render(ID3D12GraphicsCommandList*) override;

	friend PipelineInterface& operator<<(PipelineInterface&, QuadClass&);

private:
	BufferType	m_vertexBuffer =	BufferType();
	BufferType	m_indexBuffer =		BufferType();
	BufferType	m_instanceBuffer =	BufferType();
};
