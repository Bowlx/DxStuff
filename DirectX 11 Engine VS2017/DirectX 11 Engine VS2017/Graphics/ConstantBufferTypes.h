#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX  WVP;
	DirectX::XMMATRIX  World;
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};

struct CB_PS_lightBuffer
{
	DirectX::XMFLOAT3 dir;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
};