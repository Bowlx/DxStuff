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

float4 main(PS_INPUT input) : SV_TARGET
{
  input.normal = normalize(input.normal);
float4 tex = objTexture.Sample(objSamplerState, input.inTexCoord);
float3 finalColor;

finalColor = tex * ambient;
finalColor += saturate(dot(dir, input.normal)  * diffuse * tex);
return float4(finalColor,1.0);

}