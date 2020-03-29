cbuffer lightBuffer : register(b0)
{
    float3 dir;
    float4 ambient;
    float4 diffuse;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 normal : NORMAL;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);
SamplerState clampSamplerState : SAMPLER1: register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
  input.normal = normalize(input.normal);
float4 tex = objTexture.Sample(objSamplerState, input.inTexCoord);
return float4(tex);

}