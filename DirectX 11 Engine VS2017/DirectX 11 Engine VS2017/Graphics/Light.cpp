#include "Light.h"


Light::Light()
{
}


void Light::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, nearZ, farZ);
	XMVECTOR d = XMVectorSet(15.0f, 25.0f, 15.0f, 0.0f);
	this->pos = XMFLOAT3(15, 25, 0);
	XMVECTOR dd = XMVectorSet(15.0f, 1.0f, 15.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f);
	this->viewMatrix = XMMatrixLookAtLH(d, dd, Up);
	
}

ID3D11ShaderResourceView* Light::GetShaderResourceView()
{
	return SRV;
}


bool Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = mapSize;
	textureDesc.Height = mapSize;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	device->CreateTexture2D(&textureDesc, nullptr,RTTexture.GetAddressOf());

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(RTTexture.Get(), &renderTargetViewDesc, RTV.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(RTTexture.Get(), &srvDesc, &SRV);


	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = mapSize;
	depthBufferDesc.Height = mapSize;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	device->CreateTexture2D(&depthBufferDesc, nullptr, DSTexture.GetAddressOf());

	
	
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(DSTexture.Get(), &depthStencilViewDesc, DSV.GetAddressOf());





	viewport = {};
	viewport.Width = mapSize;
	viewport.Height = mapSize;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	this->lightbuffer.Initialize(device, deviceContext);
	return true;
	
}


void Light::Update()
{
	float dist = 165;
	
	
	
}

void Light::Draw(ID3D11DeviceContext* deviceContext)
{


}

void Light::setRenderTarget(ID3D11DeviceContext* deviceContext)
{
	
	deviceContext->OMSetRenderTargets(1, RTV.GetAddressOf(), DSV.Get());
	deviceContext->RSSetViewports(1, &viewport);
}

void Light::ClearRenderTarget(ID3D11DeviceContext* deviceContext)
{
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	deviceContext->ClearRenderTargetView(RTV.Get(),bgcolor);
	deviceContext->ClearDepthStencilView(DSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Light::setShaderResources(ID3D11DeviceContext* deviceContext)
{
	lightbuffer.data.lightposition = pos;
	lightbuffer.ApplyChanges();
	deviceContext->VSSetConstantBuffers(1, 1, this->lightbuffer.GetAddressOf());
	deviceContext->PSSetShaderResources(1, 1,&this->SRV);
}
