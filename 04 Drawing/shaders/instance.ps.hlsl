////////////////////////////////////////////////////////////////////////////////
// Filename: instance.ps.hlsl
////////////////////////////////////////////////////////////////////////////////


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float3 HSV : COLOR1;
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
	float3 RGB = HUEtoRGB(HSV.r);
	return ((RGB - 1) * HSV.g + 1) * HSV.b;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMain(PixelInputType input) : SV_TARGET
{
	float4 textureHSVA, outputRGBA;
	float3 tempRGB;


	// Sample the texture using the sampler at this texture coordinate location.
	textureHSVA = input.color;

	// Modify the saturation and value of our pixel color using the instance input colors.
	textureHSVA.r = input.HSV.r;
	textureHSVA.gb *= input.HSV.gb;

	// Calculate the RGB values of our pixel from the input HSV
	tempRGB = HSVtoRGB(textureHSVA.rgb);

	// Combine the RGB we just got with our alpha channel to get the final color.
	outputRGBA = float4(tempRGB, textureHSVA.a);

	return outputRGBA;
}
