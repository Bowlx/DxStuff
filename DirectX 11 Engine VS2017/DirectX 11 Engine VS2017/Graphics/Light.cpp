#include "Light.h"


Light::Light()
{
}


DirectX::XMMATRIX Light::getViewProjectionMatrix()
{
	return viewProjectionMatrix;
}


bool Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = mapSize;
	depthDesc.Height = mapSize;
	depthDesc.ArraySize = 1;
	depthDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.MipLevels = 1;
	depthDesc.MipLevels = 0;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	device->CreateTexture2D(&depthDesc, nullptr, &RT_texture);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = depthDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(RT_texture.Get(), &renderTargetViewDesc, RTV.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(RT_texture.Get(), &srvDesc, DepthshaderView.GetAddressOf());


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
	device->CreateTexture2D(&depthBufferDesc, nullptr, DS_texture.GetAddressOf());

	
	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc = {};
	dViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dViewDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(RT_texture.Get(), &dViewDesc, depthStencilView.GetAddressOf());



	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.MultisampleEnable = true;
	rastDesc.DepthBias = 1;
	rastDesc.SlopeScaledDepthBias = 1.5f;
	rastDesc.DepthBiasClamp = 4;
	device->CreateRasterizerState(&rastDesc, rastState.GetAddressOf());

	viewport = {};
	viewport.Width = mapSize;
	viewport.Height = mapSize;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	return true;
}


void Light::Update()
{
	float dist = 165;
	
	viewProjectionMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(dist,dist,dist)), DirectX::XMVectorZero(), DEFAULT_UP_VECTOR) * DirectX::XMMatrixOrthographicOffCenterLH(0,1024,0,1024,0.1f,750.0f);
	
	
}

void Light::Draw(ID3D11DeviceContext* deviceContext)
{
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->OMSetRenderTargets(0, nullptr, depthStencilView.Get());
	//deviceContext->RSSetState(rastState.Get());
	deviceContext->RSSetViewports(1, &viewport);

}

void Light::setRenderTarget(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetRenderTargets(1, RTV.GetAddressOf(), depthStencilView.Get());
	deviceContext->RSSetViewports(1, &viewport);
}

void Light::ClearRenderTarget(ID3D11DeviceContext* deviceContext)
{
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	deviceContext->ClearRenderTargetView(RTV.Get(),bgcolor);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
