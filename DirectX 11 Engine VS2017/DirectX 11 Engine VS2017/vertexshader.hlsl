cbuffer mycBuffer : register(b0)
{
    float4x4 World;
    float4x4 WVP;
    float4x4 WVPlight;
}; 

cbuffer lightBuffer : register(b1)
{
    
    float3 lightposition;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 normal : NORMAL;
    float2 inTexCoord : TEXCOORD;
  
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float4 lightViewPosition : TEXCOORD1;
    float3 normal : NORMAL;
    float3 lightPos : TEXCOORD2;
    float2 outTexCoord : TEXCOORD0;

};

VS_OUTPUT main(VS_INPUT input) 
{
    VS_OUTPUT output;

    output.outPosition = mul(float4(input.inPos, 1.0f), WVP);
    output.lightViewPosition = mul(float4(input.inPos, 1.0f), WVPlight);

    output.outTexCoord = input.inTexCoord;

    output.normal = mul(input.normal, World);
    output.normal = normalize(output.normal);
    

    float4 worldPosition = mul(input.inPos, World);

    output.lightPos = lightposition.xyz - worldPosition.xyz;
    output.lightPos = normalize(output.lightPos);

    return output;
}