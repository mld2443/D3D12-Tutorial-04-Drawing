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
		SIZE_T					count =		0;
		ComPtr<ID3D12Resource>	buffer =	nullptr;
		union
		{
			D3D12_VERTEX_BUFFER_VIEW	vertexView;
			D3D12_INDEX_BUFFER_VIEW		indexView =	D3D12_INDEX_BUFFER_VIEW();
		};

		BufferType() = default;
		BufferType(ID3D12Device*, vector<UINT>, wstring = L"GI index buffer");
		template<typename Type>
		BufferType(ID3D12Device*, vector<Type>, wstring = L"GI vertex buffer");

	private:
		static ComPtr<ID3D12Resource> InitializeBuffer(ID3D12Device*, BYTE*, SIZE_T, D3D12_RESOURCE_STATES, wstring);
	};

public:
	GeometryInterface() = delete;
	GeometryInterface(const GeometryInterface&) = delete;
	GeometryInterface& operator=(const GeometryInterface&) = delete;

	GeometryInterface(pipeline_func);
	~GeometryInterface() = default;

public:
	const pipeline_func Render;
};


///////////////////////////////
// INLINE TEMPLATE FUNCTIONS //
///////////////////////////////
template<typename Type>
inline GeometryInterface::BufferType::BufferType(ID3D12Device* device, vector<Type> data, wstring name) :
	count(data.size()),
	buffer(InitializeBuffer(
		device,
		reinterpret_cast<BYTE*>(data.data()),
		count * sizeof(Type),
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		name
	)),
	vertexView{
		buffer->GetGPUVirtualAddress(),
		static_cast<UINT>(count * sizeof(Type)),
		static_cast<UINT>(sizeof(Type))
	}
{
}
