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
        XMFLOAT3 position = {};
        XMFLOAT4 color    = {};
    };

public:
    TriangleClass() = delete;
    TriangleClass(const TriangleClass &) = delete;
    TriangleClass & operator=(const TriangleClass &) = delete;

    TriangleClass(ID3D12Device *);
    ~TriangleClass() = default;

    void Render(ID3D12GraphicsCommandList *) override;

private:
    BufferType m_vertexBuffer = {};
    BufferType m_indexBuffer  = {};
};
