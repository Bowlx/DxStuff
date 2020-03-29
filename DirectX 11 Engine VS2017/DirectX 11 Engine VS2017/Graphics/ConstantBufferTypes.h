#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	
	DirectX::XMMATRIX  World;
	DirectX::XMMATRIX  WVP;
	DirectX::XMMATRIX  WVPlight;
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};

struct CB_PS_lightBuffer
{
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;

	
};

struct lightbuff
{
	DirectX::XMFLOAT3 lightposition;

	
};