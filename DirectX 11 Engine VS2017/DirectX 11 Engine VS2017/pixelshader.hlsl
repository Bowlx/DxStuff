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
	float4 color;

};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D depthMapTexture : TEXTURE1 : register(t1);
Texture2D curLumiTexture : TEXTURE2: register(t2);
Texture2D prevLumiTexture : TEXTURE2: register(t3);

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
float4 textureColor1;
// ”становка значени€ смещени€ используемого дл€ устранени€ проблем точности с плавающей зап€то

float bias = 0.001f;
float4 color = ambient;

// ¬ычисление координат проецировани€ текстуры


projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

// Ќаходитс€ ли спроецированные координаты в пределах 0 и 1. ≈сли да, то пиксель находитс€ в видимости света

if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
{
	depthValue = depthMapTexture.Sample(clampSamplerState, projectTexCoord).r;

	// ¬ычисление глубины света
	lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

	// ¬ычитание смещени€ из lightDepthValue
	
	lightDepthValue = lightDepthValue - bias;
	// —равнение глубины теневой карты и глубины света, дл€ определени€ того, освещен или затенен пиксель
	// ≈сли свет перед объектом, то пиксель освещен; если нет, то пиксель затенен и объект бросает тень за ним
	if (lightDepthValue < depthValue)
	{
		// ¬ычисление количества света в пикселе
		lightIntensity = saturate(dot(input.normal, input.lightPos));
		if (lightIntensity > 0.0f)
		{
			
			
			// ќпределение заключительного рассе€ного (diffuse) света на основе рассе€ного цвети и интенсивности света
			color += (diffuse * lightIntensity);
			
			color = saturate(color);
		}
	}
	
}





float  lum = curLumiTexture.Load(int3(0, 0, 10 ));
float  prevlum = prevLumiTexture.Load(int3(0, 0, 10));

textureColor = objTexture.Sample(objSamplerState, input.inTexCoord);
float3 texcol = textureColor.rgb;

float3 ldr;

if (lum > 0) {
	
	float l = lerp(prevlum, lum, 0.25);
	ldr = float3(1.0f, 1.0f, 1.0f) - exp(-texcol * 0.1/l);
}
else {
	ldr = float3(1.0f, 1.0f, 1.0f) - exp(-texcol * 2);
}




return color * textureColor;



}