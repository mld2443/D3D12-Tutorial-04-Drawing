////////////////////////////////////////////////////////////////////////////////
// Filename: instance.vs.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position         : POSITION0;
    float4 color            : COLOR0;
    float4 instancePosition : POSITION1;
    float3 instanceHSV      : COLOR1;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color    : COLOR0;
    float3 HSV      : COLOR1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VSMain(VertexInputType input)
{
    // Convert the position vector to homogeneous coordinates for matrix calculations.
    input.position.w = 1.0f;

    // Update the position of the vertices based on the data for this particular instance.
    input.position.xyz += input.instancePosition.xyz;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    PixelInputType output;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the input color for the pixel shader to use.
    output.color = input.color;

    // Pass along the HSV for the pixel shader to use.
    output.HSV = input.instanceHSV;

    return output;
}
