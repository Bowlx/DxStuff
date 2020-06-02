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
	DirectX::XMFLOAT3 cameraPosition;
	DirectX::XMFLOAT3 lightColor;
	DirectX::XMFLOAT3 lightPos;

	
};
struct CB_PS_DirlightBuffer
{

	DirectX::XMFLOAT3 lightColor;
	DirectX::XMFLOAT3 lightDir;


};

struct lightbuff
{
	DirectX::XMFLOAT3 lightposition;

	
};