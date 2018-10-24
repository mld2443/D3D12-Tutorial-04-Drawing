////////////////////////////////////////////////////////////////////////////////
// Filename: geometryinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <d3d12.h>
#include <directxmath.h>
#include <vector>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Interface name: GeometryInterface
////////////////////////////////////////////////////////////////////////////////
class GeometryInterface
{
protected:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	GeometryInterface();
	GeometryInterface(const GeometryInterface&);
	~GeometryInterface();

	virtual bool Initialize(ID3D12Device*) = 0;
	virtual void Shutdown() = 0;
	void Render();

	unsigned int GetVertexCount();
	unsigned int GetIndexCount();

private:
	template<typename T>
	bool CreateBuffer(ID3D12Device*, ID3D12Resource*, const std::vector<T>&, D3D12_RESOURCE_STATES, LPCWSTR = L"");

protected:
	bool InitializeVertexBuffer(ID3D12Device*, const std::vector<VertexType>&);
	bool InitializeIndexBuffer(ID3D12Device*, const std::vector<unsigned long>&);

	void ShutdownBuffers();

private:
	ID3D12Resource	*m_vertexBuffer, *m_indexBuffer;
	unsigned int	m_vertexCount, m_indexCount;
};
