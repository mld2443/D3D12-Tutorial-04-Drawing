////////////////////////////////////////////////////////////////////////////////
// Filename: instance.ps.hlsl
////////////////////////////////////////////////////////////////////////////////


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color    : COLOR0;
    float3 HSV      : COLOR1;
};


////////////////////////////////////////////////////////////////////////////////
// Color Conversions
////////////////////////////////////////////////////////////////////////////////
// NOTE: Color Functions courtesy of Ian Taylor
// http://www.chilliant.com/rgb2hsv.html

float3 HUEtoRGB(in float H)
{
    float R = abs(H * 6.0f - 3.0f) - 1.0f;
    float G = 2.0f - abs(H * 6.0f - 2.0f);
    float B = 2.0f - abs(H * 6.0f - 4.0f);
    return saturate(float3(R, G, B));
}

float3 HSVtoRGB(in float3 HSV)
{
    float3 RGB = HUEtoRGB(HSV.x);
    return ((RGB - 1.0f) * HSV.y + 1.0f) * HSV.z;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMain(PixelInputType input) : SV_TARGET
{
    float4 HSVA;


    // Sample the texture using the sampler at this texture coordinate location.
    HSVA = input.color;

    // Modify the saturation and value of our pixel color using the instance input colors.
    HSVA.x   = input.HSV.x;
    HSVA.yz *= input.HSV.yz;

    // Calculate the RGB values of our pixel from the input HSV
    // Combine the RGB we just got with our alpha channel to get the final color.
    return float4(HSVtoRGB(HSVA.xyz), HSVA.a);
}
