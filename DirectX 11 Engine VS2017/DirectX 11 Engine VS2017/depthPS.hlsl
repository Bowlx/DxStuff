struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};


float4 main(PS_INPUT input) : SV_POSITION
{
	// Получаем значение глубины пикселя деля Z пикселя глубины на гомогенную координату W
	float depthValue = input.depthPosition.z / input.depthPosition.w;

	return float4(depthValue, depthValue, depthValue, 1.0f);
}