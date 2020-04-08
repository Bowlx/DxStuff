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

Texture2D objTexture : TEXTURE : register(t0);
Texture2D depthMapTexture : TEXTURE1 : register(t1);

SamplerState objSamplerState  : register(s0);
SamplerState clampSamplerState : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
	/* input.normal = normalize(input.normal);
   float4 tex = objTexture.Sample(objSamplerState, input.inTexCoord);
   return float4(tex);*/

float2 projectTexCoord;
float depthValue;
float lightDepthValue;
float lightIntensity;
float4 textureColor;

// ��������� �������� �������� ������������� ��� ���������� ������� �������� � ��������� �������
float bias = 0.001f;

float4 color = ambient;

// ���������� ��������� ������������� ��������


projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

// ��������� �� ��������������� ���������� � �������� 0 � 1. ���� ��, �� ������� ��������� � ��������� �����

if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
{
	depthValue = depthMapTexture.Sample(clampSamplerState, projectTexCoord).r;

	// ���������� ������� �����
	lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

	// ��������� �������� �� lightDepthValue
	
	lightDepthValue = lightDepthValue - bias;
	// ��������� ������� ������� ����� � ������� �����, ��� ����������� ����, ������� ��� ������� �������
	// ���� ���� ����� ��������, �� ������� �������; ���� ���, �� ������� ������� � ������ ������� ���� �� ���
	if (lightDepthValue < depthValue)
	{
		// ���������� ���������� ����� � �������
		lightIntensity = saturate(dot(input.normal, input.lightPos));
		lightDepthValue = lightDepthValue - bias ;
		if (lightIntensity > 0.0f)
		{
			
			
			// ����������� ��������������� ���������� (diffuse) ����� �� ������ ���������� ����� � ������������� �����
			color += (diffuse * lightIntensity);
			
			color = saturate(color);
		}
	}
	
}




textureColor = objTexture.Sample(objSamplerState, input.inTexCoord);


return  color* textureColor;

}