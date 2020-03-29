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

	Microsoft::WRL::ComPtr<ID3D11Texture2D> RTTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DSTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV;


	
	D3D11_VIEWPORT viewport;
	ConstantBuffer<lightbuff> lightbuffer;


	
public:
	Light();
	ID3D11ShaderResourceView* SRV; 
	int mapSize = 8192;
	DirectX::XMMATRIX getViewProjectionMatrix();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	ID3D11ShaderResourceView* GetShaderResourceView();
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Update();
	void Draw(ID3D11DeviceContext* deviceContext);
	void setRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext);
	void setShaderResources(ID3D11DeviceContext* deviceContext);

};
