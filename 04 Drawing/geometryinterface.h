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
	GeometryInterface(const GeometryInterface&) = delete;
	GeometryInterface& operator=(const GeometryInterface&) = delete;

	GeometryInterface() = default;
	~GeometryInterface();

	UINT GetVertexCount();
	UINT GetIndexCount();

	void Render(ID3D12GraphicsCommandList*);

protected:
	void InitializeVertexBuffer(ID3D12Device*, const vector<VertexType>&);
	void InitializeIndexBuffer(ID3D12Device*, const vector<UINT32>&);

private:
	template<typename BufferType>
	static void InitializeBuffer(ID3D12Device*, ID3D12Resource**, const vector<BufferType>&, D3D12_RESOURCE_STATES, wstring = L"GI buffer");

private:
	ID3D12Resource*				m_vertexBuffer =		nullptr;
	ID3D12Resource*				m_indexBuffer =			nullptr;
	UINT						m_vertexCount =			0;
	UINT						m_indexCount =			0;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView =	D3D12_VERTEX_BUFFER_VIEW();
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView =		D3D12_INDEX_BUFFER_VIEW();
};
