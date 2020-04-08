cbuffer mycBuffer : register(b0)
{

	matrix World;
	matrix WVP;
	matrix WVPlight;
};
cbuffer lightBuffer : register(b1)
{

	float3 lightposition;
};

struct VS_INPUT
{
	float3 inPos : POSITION;
	float2 inTexCoord : TEXCOORD0;
	float3 normal : NORMAL;

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