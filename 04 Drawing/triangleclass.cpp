////////////////////////////////////////////////////////////////////////////////
// Filename: triangleclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "triangleclass.h"


TriangleClass::TriangleClass(ID3D12Device* device) :
    GeometryInterface(
[=](ID3D12GraphicsCommandList* commandList)
{
    // Set the type of primitive that the input assembler will try to assemble next.
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set the vertex and index buffers as active in the input assembler so they will be used for rendering.
    commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer.vertexView);
    commandList->IASetIndexBuffer(&m_indexBuffer.indexView);

    // Issue the draw call for this geometry.
    commandList->DrawIndexedInstanced(static_cast<UINT>(m_indexBuffer.count), 1, 0, 0, 0);
})
{
    vector<VertexType> vertices;
    vector<UINT32> indices;


    // Set the sizes of our vectors.
    vertices.resize(3);
    indices.resize(3);

    // Load the vertex vector with points of a triangle.
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);    // Bottom left.
    vertices[0].color    = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);    // Top middle.
    vertices[1].color    = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);    // Bottom right.
    vertices[2].color    = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    // Load the index vector with data.
    indices[0] = 0;  // Bottom left.
    indices[1] = 1;  // Top middle.
    indices[2] = 2;  // Bottom right.

    // Initialize the vertex and index buffers.
    m_vertexBuffer = BufferType(device, vertices, L"TC vertex buffer");
    m_indexBuffer  = BufferType(device, indices, L"TC index buffer");
}
