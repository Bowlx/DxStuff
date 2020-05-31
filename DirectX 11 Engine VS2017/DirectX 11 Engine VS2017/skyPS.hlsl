cbuffer lightBuffer : register(b0)
{
	float4 diffuse;
	float4 ambient;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
	float4 lightViewPosition : TEXCOORD1;
    float3 normal : NORMAL;
    float3 lightPos : TEXCOORD2;
	float2 inTexCoord : TEXCOORD0;
};

struct PS_OUTPUT {
	float4 color : SV_TARGET0;
	float4 lum : SV_TARGET1;
};

TextureCube objTexture : TEXTURE : register(t0);
Texture2D depthMapTexture : TEXTURE1 : register(t1);
Texture2D curLumiTexture : TEXTURE2: register(t2);
Texture2D prevLumiTexture : TEXTURE2: register(t3);

SamplerState objSamplerState  : register(s0);
SamplerState clampSamplerState : register(s1);

PS_OUTPUT main(PS_INPUT input)
{
float4 textureColor;
PS_OUTPUT output;

float3 refnormals = input.normal.xyz;

textureColor = objTexture.Sample(objSamplerState, float4(refnormals,1.0));

output.color = textureColor;
output.lum = textureColor;
return output;



}