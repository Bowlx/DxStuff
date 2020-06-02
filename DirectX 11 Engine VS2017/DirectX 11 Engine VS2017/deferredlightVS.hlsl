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

struct VertexToPixel
{
    float4 position		: SV_POSITION;
};

VertexToPixel main(VS_INPUT input)
{
    VertexToPixel output;

    output.position = mul(float4(input.inPos, 1.0f), WVP);


    return output;
}