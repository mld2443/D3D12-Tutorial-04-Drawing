////////////////////////////////////////////////////////////////////////////////
// Filename: geometryinterface.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


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
	void Render(ID3D12GraphicsCommandList*);

	unsigned long GetVertexCount();
	unsigned long GetIndexCount();

private:
	template<typename T>
	bool CreateBuffer(ID3D12Device*, ID3D12Resource**, const std::vector<T>&, D3D12_RESOURCE_STATES, std::wstring = L"GI buffer");

protected:
	bool InitializeVertexBuffer(ID3D12Device*, const std::vector<VertexType>&);
	bool InitializeIndexBuffer(ID3D12Device*, const std::vector<unsigned long>&);

	void ShutdownBuffers();

private:
	ID3D12Resource				*m_vertexBuffer, *m_indexBuffer;
	unsigned long				m_vertexCount, m_indexCount;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView;
};
