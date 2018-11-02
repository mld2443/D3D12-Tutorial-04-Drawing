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
	struct BufferType
	{
		ComPtr<ID3D12Resource>	buffer =	nullptr;
		SIZE_T					count =		0;
		union
		{
			D3D12_VERTEX_BUFFER_VIEW	vertexView;
			D3D12_INDEX_BUFFER_VIEW		indexView =	D3D12_INDEX_BUFFER_VIEW();
		};

		BufferType() = default;
		BufferType(ID3D12Device*, BYTE*, SIZE_T, SIZE_T, wstring = L"GI vertex buffer");
		BufferType(ID3D12Device*, BYTE*, SIZE_T, SIZE_T, DXGI_FORMAT, wstring = L"GI index buffer");

	private:
		BufferType(ID3D12Device*, BYTE*, SIZE_T, SIZE_T, D3D12_RESOURCE_STATES, wstring);
	};

public:
	GeometryInterface(const GeometryInterface&) = delete;
	GeometryInterface& operator=(const GeometryInterface&) = delete;

	GeometryInterface() = default;
	~GeometryInterface() = default;

	virtual void Render(ID3D12GraphicsCommandList*) = 0;

private:
	static ComPtr<ID3D12Resource> InitializeBuffer(ID3D12Device*, BYTE*, SIZE_T, D3D12_RESOURCE_STATES, wstring);
};
