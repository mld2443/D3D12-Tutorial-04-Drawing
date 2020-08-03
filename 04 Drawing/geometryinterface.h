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
        SIZE_T                 count  = 0ull;
        ComPtr<ID3D12Resource> buffer = nullptr;
        union
        {
            D3D12_VERTEX_BUFFER_VIEW vertexView;
            D3D12_INDEX_BUFFER_VIEW  indexView = {};
        };

        BufferType() = default;
        BufferType(ID3D12Device *, std::vector<uint32_t> &, const std::wstring = L"GI index buffer");
        template<typename Type>
        BufferType(ID3D12Device *, std::vector<Type> &, const std::wstring = L"GI vertex buffer");

    private:
        static ComPtr<ID3D12Resource> InitializeBuffer(ID3D12Device *, BYTE *, SIZE_T, D3D12_RESOURCE_STATES, const std::wstring);
    };

public:
    GeometryInterface(const GeometryInterface &) = delete;
    GeometryInterface & operator=(const GeometryInterface &) = delete;

    GeometryInterface() = default;
    virtual ~GeometryInterface() = default;

    virtual void Render(ID3D12GraphicsCommandList *) = 0;
};


///////////////////////////////
// INLINE TEMPLATE FUNCTIONS //
///////////////////////////////
template<typename Type>
GeometryInterface::BufferType::BufferType(ID3D12Device *device, std::vector<Type> &data, const std::wstring name)
    : count(data.size())
    , buffer(InitializeBuffer(device,
                              reinterpret_cast<BYTE*>(data.data()),
                              count * sizeof(Type),
                              D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                              name))
    , vertexView{ buffer->GetGPUVirtualAddress(),
                  static_cast<UINT>(count * sizeof(Type)),
                  static_cast<UINT>(sizeof(Type)) }
{
}
