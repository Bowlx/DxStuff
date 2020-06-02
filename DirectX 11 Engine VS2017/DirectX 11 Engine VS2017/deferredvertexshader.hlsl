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

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float4 lightViewPosition : TEXCOORD1;
    float4 outWorld: POSITIONWS;
    float3 normal : NORMAL;
    float3 lightPos : TEXCOORD2;
    float3 tangent: TANGENTWS;
    float2 outTexCoord : TEXCOORD0;


};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.outPosition = mul(float4(input.inPos, 1.0f), WVP);

    output.lightViewPosition = mul(float4(input.inPos, 1.0f), WVPlight);

    output.outTexCoord = input.inTexCoord;

    output.normal = mul(input.normal, (float3x3)World);
    float4 worldPosition = float4(mul(float4(input.inPos, 1.0), World).xyz, 1.0);

    output.outWorld = worldPosition;
    output.lightPos = lightposition.xyz - worldPosition.xyz;
    output.lightPos = normalize(output.lightPos);

    return output;
}