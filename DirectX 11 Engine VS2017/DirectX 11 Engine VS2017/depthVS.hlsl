cbuffer MatrixBuffer : register(b0)
{
	float4x4 WVP;
	float4x4 World;
};

struct VertexInputType
{
	float4 inPosition : POSITION;
	float2 inTexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	input.inPosition.w = 1.0f;
	output.position = mul(input.inPosition, WVP);
	// Пишем позицию в depthPosition
	output.depthPosition = output.position;

	return output;
}