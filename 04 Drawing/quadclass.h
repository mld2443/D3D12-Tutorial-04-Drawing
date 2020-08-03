////////////////////////////////////////////////////////////////////////////////
// Filename: quadclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "geometryinterface.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: QuadClass
////////////////////////////////////////////////////////////////////////////////
class QuadClass : public GeometryInterface
{
private:
    struct VertexType
    {
        XMFLOAT3 position = {};
        XMFLOAT4 color    = {};
    };

    struct InstanceType
    {
        XMFLOAT3 position = {};
        XMFLOAT3 hsv      = {};
    };

public:
    QuadClass() = delete;
    QuadClass(const QuadClass &) = delete;
    QuadClass & operator=(const QuadClass &) = delete;

    QuadClass(ID3D12Device *);
    ~QuadClass() = default;

    void Render(ID3D12GraphicsCommandList *) override;

private:
    BufferType m_vertexBuffer   = {};
    BufferType m_indexBuffer    = {};
    BufferType m_instanceBuffer = {};
};
