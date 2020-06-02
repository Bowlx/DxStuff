cbuffer lightBuffer : register(b0)
{
	float4 diffuse;
	float4 ambient;
}

struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float4 lightViewPosition : TEXCOORD1;
	float4 outWorld: POSITIONWS;
	float3 normal : NORMAL;
	float3 lightPos : TEXCOORD2;

	float3 tangent	: TANGENTWS;
	float2 inTexCoord : TEXCOORD0;

};

struct PSOut
{
	float4 Position		: SV_Target0;
	float4 Normal		: SV_Target1;
	float4 Diffuse		: SV_Target2;
};

Texture2D objTexture : TEXTURE: register(t0);
Texture2D normalMapSRV : register(t4);


SamplerState objSamplerState  : register(s0);


PSOut main(PS_INPUT input) : SV_TARGET
{
	PSOut output;
input.normal = normalize(input.normal);
input.tangent = normalize(input.tangent);

	float4 diffuse = objTexture.Sample(objSamplerState, input.inTexCoord);


	//input.normal = normalize(mul(normalFromMap, TBN));



	output.Position = float4(input.outWorld.xyz, 1.0f);
	output.Normal = float4(input.normal, 1.0f);
	output.Diffuse = diffuse;
	return output;
}