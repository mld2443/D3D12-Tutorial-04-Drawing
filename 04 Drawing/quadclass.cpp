////////////////////////////////////////////////////////////////////////////////
// Filename: quadclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "quadclass.h"


QuadClass::QuadClass(ID3D12Device *device)
{
    // Create the containers that we will build our geoemetry inside.
    std::vector<VertexType>   vertices;
    std::vector<uint32_t>     indices;
    std::vector<InstanceType> instances;

    // Preset the sizes of our vectors.
    vertices.resize(4);
    indices.resize(6);
    instances.resize(4);

    // Load the vertex vector with points of a square.
    vertices[0].position = { -1.0f, 1.0f, 0.0f };   // Top left.
    vertices[0].color    = { 1.0f, 0.0f, 1.0f, 1.0f };

    vertices[1].position = { 1.0f, 1.0f, 0.0f };    // Top right.
    vertices[1].color    = { 1.0f, 0.0f, 0.0f, 1.0f };

    vertices[2].position = { 1.0f, -1.0f, 0.0f };   // Bottom right.
    vertices[2].color    = { 1.0f, 1.0f, 0.0f, 1.0f };

    vertices[3].position = { -1.0f, -1.0f, 0.0f };  // Bottom left.
    vertices[3].color    = { 1.0f, 1.0f, 1.0f, 1.0f };

    // Describe the two triangles that make up a square.
    indices[0] = 0u;  // Top left.
    indices[1] = 1u;  // Top right.
    indices[2] = 2u;  // Bottom right.

    indices[3] = 0u;  // Top left.
    indices[4] = 2u;  // Bottom right.
    indices[5] = 3u;  // Bottom left.

    // Set four distinct copies of this square.
    instances[0].position = { -2.0f, 2.0f, 0.0f };
    instances[0].hsv      = { 0.25f, 1.0f, 1.0f };

    instances[1].position = { 2.0f, 2.0f, 0.0f };
    instances[1].hsv      = { 0.5f, 1.0f, 1.0f };

    instances[2].position = { 2.0f, -2.0f, 0.0f };
    instances[2].hsv      = { 0.75f, 1.0f, 1.0f };

    instances[3].position = { -2.0f, -2.0f, 0.0f };
    instances[3].hsv      = { 1.0f, 1.0f, 1.0f };

    // Initialize the buffers and their views.
    m_vertexBuffer   = BufferType(device, vertices,  L"QC vertex buffer");
    m_indexBuffer    = BufferType(device, indices,   L"QC index buffer");
    m_instanceBuffer = BufferType(device, instances, L"QC instance buffer");
}


void QuadClass::Render(ID3D12GraphicsCommandList *commandList)
{
    // Set the views associated with this geometry vertex buffers.
    D3D12_VERTEX_BUFFER_VIEW pViews[2];
    pViews[0] = m_vertexBuffer.vertexView;
    pViews[1] = m_instanceBuffer.vertexView;

    // Set the vertex and index buffers as active in the input assembler so they will be used for
    // rendering.
    commandList->IASetVertexBuffers(0, 2, pViews);
    commandList->IASetIndexBuffer(&m_indexBuffer.indexView);

    // Set the type of primitive that the input assembler will try to assemble next.
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Issue the draw call for this geometry.
    commandList->DrawIndexedInstanced(static_cast<UINT>(m_indexBuffer.count), static_cast<UINT>(m_instanceBuffer.count), 0u, 0, 0u);
}
