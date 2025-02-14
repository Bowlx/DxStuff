Texture2D Texture							: register(t1);
Texture2D NormalMap							: register(t2);
Texture2D PositionTexture					: register(t3);

SamplerState basicSampler : register(s0);

cbuffer LightData		: register(b0)
{
	float3 cameraPosition;
	float3 lightColor;
	float3 lightPosition;


}

struct VertexToPixel
{
	float4 position		: SV_POSITION;

};

float4 main(in VertexToPixel input) : SV_TARGET0
{

	int3 sampleIndices = int3(input.position.xy, 0);

	float3 normal = NormalMap.Load(sampleIndices).xyz;
	float3 position = PositionTexture.Load(sampleIndices).xyz;
	float3 diffuse = Texture.Load(sampleIndices).xyz;

	float3 L = lightPosition - position;
	float dist = length(L);

	if (dist > 2.0f)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	L /= dist;
	float distCoef = 1.8f;
	float att = max(0.0f, 1.0f - (dist / distCoef));

	float lightAmount = saturate(dot(normal, L));
	float3 color = lightAmount * lightColor * att;

	//Specular calc
	float3 V = cameraPosition - position;
	float3 H = normalize(L + V);
	float specular = pow(saturate(dot(normal, H)), 10) * att;

	float3 finalDiffuse = color * diffuse;
	float3 finalSpecular = specular * diffuse * att;

	float4 totalColor = float4(finalDiffuse + finalSpecular, 1.0f);
	return totalColor;


}