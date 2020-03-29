#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "Shaders.h"
#include "ConstantBufferTypes.h"
#include "ConstantBuffer.h"
#include "Camera.h"


class Light : public Camera
{

	Microsoft::WRL::ComPtr<ID3D11Texture2D> RT_texture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DS_texture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;

	
	D3D11_VIEWPORT viewport;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	DirectX::XMMATRIX viewProjectionMatrix;

	
public:
	Light();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DepthshaderView; 
	int mapSize = 4096;
	DirectX::XMMATRIX getViewProjectionMatrix();
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Update();
	void Draw(ID3D11DeviceContext* deviceContext);
	void setRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext);

};
