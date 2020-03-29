cbuffer mycBuffer : register(b0)
{

	float4x4 World;
	float4x4 WVP;
	float4x4 WVPlight;
};

struct VS_INPUT
{
	float3 inPos : POSITION;
	float3 normal : NORMAL;
	float2 inTexCoord : TEXCOORD;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};

PixelInputType main(VS_INPUT input)
{
	PixelInputType output;


	output.position = mul(float4(input.inPos,1.0f), WVPlight);
	// Пишем позицию в depthPosition
	output.depthPosition = output.position;

	return output;
}