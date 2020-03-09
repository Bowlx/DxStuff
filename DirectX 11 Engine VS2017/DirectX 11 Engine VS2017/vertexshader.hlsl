cbuffer mycBuffer : register(b0)
{
    float4x4 WVP;
    float4x4 World;
}; 

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 normal : NORMAL;
};

VS_OUTPUT main(VS_INPUT input) 
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), WVP);
    output.normal = mul(input.normal, World);
    output.outTexCoord = input.inTexCoord;
    return output;
}