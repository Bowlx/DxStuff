#include "Graphics.h"
#include <random>

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->fpsTimer.Start();
	this->deltaTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;
	
	if (!InitializeScene())
		return false;
	if (!Initialize2dStuff(hwnd))
		return false;
	if (!light.Initialize(this->device.Get(), this->deviceContext.Get()))
		return false;
	if (!defferedInitialize())
		return false;
	
	return true;
}

void Graphics::RenderFrame()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float blend[4] = { 1,1,1,1 };
	renderDeferredGeometry();
	

	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	deviceContext->RSSetState(rasterizerState.Get());

	deviceContext->OMSetBlendState(blendState.Get(), blend, 0xFFFFFFFF);
	deviceContext->OMSetDepthStencilState(depthStateDR, 0);
	
	ID3D11Buffer* nothing = 0;
	this->deviceContext->PSSetShader(dirlightPS.GetShader(), NULL, 0);
	this->deviceContext->VSSetShader(dirlightVS.GetShader(), NULL, 0);
	deviceContext->PSSetShaderResources(0, 3, shaderResourceViewArray);
	
	this->deviceContext->IASetInputLayout(this->dirlightVS.GetInputLayout());
	deviceContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	deviceContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
	
	
	
	
	
	this->cb_ps_dirlightBuffer.data.lightColor = XMFLOAT3(0.5f, 0.5f, 0.5f);
	this->cb_ps_dirlightBuffer.data.lightDir = XMFLOAT3(0, 0, -10);
	this->cb_ps_dirlightBuffer.ApplyChanges();
	deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_dirlightBuffer.GetAddressOf());


	


	deviceContext->Draw(3, 0);

	
	
	this->deviceContext->VSSetShader(deferredlightVS.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(deferredlightPS.GetShader(), NULL, 0);
	this->deviceContext->IASetInputLayout(this->deferredlightVS.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	//this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	

	

	


	


	for (auto i = 0; i < lights.size(); i++)
	{


		
		this->cb_ps_lightBuffer.data.cameraPosition = camera.GetPositionFloat3();
		this->cb_ps_lightBuffer.data.lightColor = lights[i].lightcolor;
		this->cb_ps_lightBuffer.data.lightPos = lights[i].GetPositionFloat3();
		this->cb_ps_lightBuffer.ApplyChanges();
		deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_lightBuffer.GetAddressOf());
		
		deviceContext->PSSetShaderResources(1, 1, &shaderResourceViewArray[2]);
		deviceContext->PSSetShaderResources(2, 1, &shaderResourceViewArray[1]);
		deviceContext->PSSetShaderResources(3, 1, &shaderResourceViewArray[0]);
		lights[i].DrawToDepth(camera.GetViewMatrix() * camera.GetProjectionMatrix(), light.GetViewMatrix() * light.GetProjectionMatrix());

	}
	

deviceContext->RSSetState(NULL);
deviceContext->OMSetBlendState(NULL, blend, 0xFFFFFFFF);
deviceContext->OMSetDepthStencilState(NULL, 0);
	this->swapchain->Present(0, 0);


}

void Graphics::restoreTargets()
{
	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);
	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->PSSetSamplers(1, 1, this->clampSamplerState.GetAddressOf());

	ID3D11RenderTargetView* rtv[2] = { renderTargetView.Get(),
	RTV.Get() };
	
	this->deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), this->depthStencilView.Get());
	//this->deviceContext->OMSetRenderTargets(2, rtv, this->depthStencilView.Get());
	this->deviceContext->RSSetViewports(1, &viewport);
}
void Graphics::restoreTargetsLum()
{
	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);
	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->PSSetSamplers(1, 1, this->clampSamplerState.GetAddressOf());
	
	this->deviceContext->OMSetRenderTargets(1, RTV.GetAddressOf(), this->depthStencilView.Get());
	this->deviceContext->RSSetViewports(1, &viewport);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No IDXGI Adapters found.");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };

		scd.BufferDesc.Width = this->windowWidth;
		scd.BufferDesc.Height = this->windowHeight;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = gammaCorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		//scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr;

		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, //FOR SOFTWARE DRIVER TYPE
			creationFlags, //FLAGS FOR RUNTIME LAYERS
			NULL, //FEATURE LEVELS ARRAY
			0, //# OF FEATURE LEVELS IN ARRAY
			D3D11_SDK_VERSION,
			&scd, //Swapchain description
			this->swapchain.GetAddressOf(), //Swapchain Address
			this->device.GetAddressOf(), //Device Address
			NULL, //Supported feature level
			this->deviceContext.GetAddressOf()); //Device Context Address

		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");
		

		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		
		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");



		D3D11_TEXTURE2D_DESC textureDesc = {};
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = this->windowWidth;
		textureDesc.Height = this->windowHeight;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		hr = this->device->CreateTexture2D(&textureDesc, nullptr, RTTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create CreateTexture2D.");
		
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		hr = this->device->CreateRenderTargetView(RTTexture.Get(), &renderTargetViewDesc, RTV.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create CreateRenderTargetView.");
		
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = -1;
		hr = this->device->CreateShaderResourceView(RTTexture.Get(), nullptr, &SRV);
		COM_ERROR_IF_FAILED(hr, "Failed to CreateShaderResourceView");

		
		//Describe our Depth/Stencil Buffer

		D3D11_TEXTURE2D_DESC depthBufferDescDR;
		ZeroMemory(&depthBufferDescDR, sizeof(depthBufferDescDR));
		depthBufferDescDR.Width = this->windowWidth;
		depthBufferDescDR.Height = this->windowHeight;
		depthBufferDescDR.MipLevels = 1;
		depthBufferDescDR.ArraySize = 1;
		//depthBufferDescDR.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDescDR.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthBufferDescDR.SampleDesc.Count = 1;
		depthBufferDescDR.SampleDesc.Quality = 0;
		depthBufferDescDR.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDescDR.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthBufferDescDR.CPUAccessFlags = 0;
		depthBufferDescDR.MiscFlags = 0;
		
		

		hr = this->device->CreateTexture2D(&depthBufferDescDR, NULL, this->depthStencilTex.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");
		
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescDR;
		ZeroMemory(&depthStencilViewDescDR, sizeof(depthStencilViewDescDR));
		depthStencilViewDescDR.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDescDR.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDescDR.Texture2D.MipSlice = 0;
		hr = this->device->CreateDepthStencilView(this->depthStencilTex.Get(), &depthStencilViewDescDR, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");
	




		
		

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");


		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
		//Create & set the Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));;
		this->viewport = viewport;
		this->viewport.TopLeftX = 0.0f;
		this->viewport.TopLeftY = 0.0f;
		this->viewport.Width = static_cast<float>(this->windowWidth);
		this->viewport.Height = static_cast<float>(this->windowHeight);
		this->viewport.MinDepth = 0.0f;
		this->viewport.MaxDepth = 1.0f;
		this->deviceContext->RSSetViewports(1, &this->viewport);
		
		//Create Rasterizer State
		D3D11_RASTERIZER_DESC rasterizerDescDR;
		ZeroMemory(&rasterizerDescDR, sizeof(rasterizerDescDR));
		rasterizerDescDR.CullMode = D3D11_CULL_BACK;
		rasterizerDescDR.FillMode = D3D11_FILL_SOLID;
		rasterizerDescDR.DepthClipEnable = false;
		hr = this->device->CreateRasterizerState(&rasterizerDescDR, this->rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Rasterizer State for culling front
	

		//Create Blend State


		D3D11_BLEND_DESC blendDescDR;
		ZeroMemory(&blendDescDR, sizeof(blendDescDR));
		blendDescDR.AlphaToCoverageEnable = false;
		blendDescDR.IndependentBlendEnable = false;
		blendDescDR.RenderTarget[0].BlendEnable = true;
		blendDescDR.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDescDR.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDescDR.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDescDR.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDescDR.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendDescDR.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDescDR.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = this->device->CreateBlendState(&blendDescDR, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\lala.spritefont");

		//Create sampler description for sampler state
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = this->device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

	
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		hr = this->device->CreateSamplerState(&samplerDesc, this->clampSamplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

		//this->restoreTargets();
		
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}


bool Graphics::InitializeShaders()
{

	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG //Debug Mode
	#ifdef _WIN64 //x64
			shaderfolder = L"..\\x64\\Debug\\";
	#else  //x86 (Win32)
			shaderfolder = L"..\\Debug\\";
	#endif
	#else //Release Mode
	#ifdef _WIN64 //x64
			shaderfolder = L"..\\x64\\Release\\";
	#else  //x86 (Win32)
			shaderfolder = L"..\\Release\\";
	#endif
#endif
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
	
	};

	D3D11_INPUT_ELEMENT_DESC layoutDR[] =
	{
		{"SV_VertexID", 0, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0, 4294967295, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	
	};


	UINT numElements = ARRAYSIZE(layout);
	UINT numElementsDR = ARRAYSIZE(layoutDR);
	

	/*if (!vertexshader.Initialize(this->device, shaderfolder + L"vertexshader.cso", layout, numElements))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"pixelshader.cso"))
		return false;
	if (!depthvertexshader.Initialize(this->device, shaderfolder + L"depthVS.cso", layout, numElements))
		return false;

	if (!depthpixelshader.Initialize(this->device, shaderfolder + L"depthPS.cso"))
		return false;
	
	if (!skyshaderPS.Initialize(this->device, shaderfolder + L"skyPS.cso"))
		return false;

	if (!skyshaderVS.Initialize(this->device, shaderfolder + L"skyVS.cso", layout, numElements))*/
		//return false;
	
	if (!deferredshaderPS.Initialize(this->device, shaderfolder + L"deferredpixelshader.cso"))
		return false;

	if (!deferredshaderVS.Initialize(this->device, shaderfolder + L"deferredvertexshader.cso", layout, numElements))
		return false;

	if (!deferredlightPS.Initialize(this->device, shaderfolder + L"deferredlightPS.cso"))
		return false;

	if (!deferredlightVS.Initialize(this->device, shaderfolder + L"deferredlightVS.cso", layout, numElements))
		return false;

	if (!dirlightPS.Initialize(this->device, shaderfolder + L"deferredDirPS.cso"))
		return false;

	if (!dirlightVS.Initialize(this->device, shaderfolder + L"deferredDirVS.cso", layoutDR, numElementsDR))
		return false;

	
	return true;
}
float get_random()
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(1, 9); // rage 0 - 1
	return dis(e);
}
bool Graphics::InitializeScene()
{
	srand(static_cast <unsigned> (time(0)));
	try
	{
		//Load Texture
		D3D11_USAGE use = {};
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\cat.jpg", nullptr, catTexture.ReleaseAndGetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create DDS texture from file.");
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\plane.tiff", nullptr, planeTexture.ReleaseAndGetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create DDS texture from file.");
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\plainNormal.png", nullptr, planeTextureNormal.ReleaseAndGetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create DDS texture from file.");
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\rock.jpg", nullptr, stonesTexture.ReleaseAndGetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create DDS texture from file.");
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\rockNormals.jpg", nullptr, stonesTextureNormal.ReleaseAndGetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create DDS texture from file.");
		
		
		
		

		hr = CreateDDSTextureFromFileEx(this->device.Get(), this->deviceContext.Get(), L"Data\\Textures\\skymap.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, false,nullptr, skyboxTexture.ReleaseAndGetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create DDS texture from file.");
		//Initialize Constant Buffer(s)
		hr = this->cb_vs_vertexshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

	/*	hr = this->cb_ps_pixelshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");*/
		hr = this->cb_ps_lightBuffer.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
		hr = this->cb_ps_dirlightBuffer.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
		if (!modelPlayer.Initialize("Data\\Objects\\spherenew.obj", this->device.Get(), this->deviceContext.Get(), this->playerTexture.Get(), this->cb_vs_vertexshader))
			return false;
		Model in;
		if (!in.Initialize("Data\\Objects\\spherenew.obj", this->device.Get(), this->deviceContext.Get(), this->stonesTexture.Get(), this->stonesTextureNormal.Get(), this->cb_vs_vertexshader))
			return false;
		Model cat;
		if (!cat.Initialize("Data\\Objects\\cat.obj", this->device.Get(), this->deviceContext.Get(), this->catTexture.Get(), this->stonesTextureNormal.Get(), this->cb_vs_vertexshader))
			return false;

		cat.SetScale(0.2f, 0.2f, 0.2f);
		cat.AdjustRotation(0, 180, 0);
		cat.SetPosition(0.5f, -1.0f, 0.0f);
		models.push_back(cat);


		if (!modelCube.Initialize("Data\\Objects\\cube.obj", this->device.Get(), this->deviceContext.Get(), this->planeTexture.Get(), this->planeTextureNormal.Get(), this->cb_vs_vertexshader))
			return false;

	
		modelCube.SetScale(0.01f, 6.5f, 5.5f);
		modelCube.SetPosition(2.5f, -2.0f, 0.5f);
		models.push_back(modelCube);

	
		in.SetPosition(2, 0, 1.3f);
		in.SetScale(5, 5, 5);
		in.lightcolor = XMFLOAT3(0.3f, 0.0f, 0.7f);
		lights.push_back(in);
	
				
	
		camera.SetPosition(0.0f, 0, -5);
		camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);
		
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

bool Graphics::defferedInitialize()
{
	int i;
	// Texture Discriptions
	D3D11_TEXTURE2D_DESC textureDescPosNorm;
	D3D11_TEXTURE2D_DESC textureDescDiffuse;

	// Texture for Position and Normal are 32 bit 
	textureDescPosNorm.Width = this->windowWidth;
	textureDescPosNorm.Height = this->windowHeight;
	textureDescPosNorm.MipLevels = 1;
	textureDescPosNorm.ArraySize = 1;
	textureDescPosNorm.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDescPosNorm.SampleDesc.Count = 1;
	textureDescPosNorm.SampleDesc.Quality = 0;
	textureDescPosNorm.Usage = D3D11_USAGE_DEFAULT;
	textureDescPosNorm.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDescPosNorm.CPUAccessFlags = 0;
	textureDescPosNorm.MiscFlags = 0;

	//Texture for diffuse is 8 bit 
	textureDescDiffuse.Width = this->windowWidth;
	textureDescDiffuse.Height = this->windowHeight;
	textureDescDiffuse.MipLevels = 1;
	textureDescDiffuse.ArraySize = 1;
	textureDescDiffuse.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDescDiffuse.SampleDesc.Count = 1;
	textureDescDiffuse.SampleDesc.Quality = 0;
	textureDescDiffuse.Usage = D3D11_USAGE_DEFAULT;
	textureDescDiffuse.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDescDiffuse.CPUAccessFlags = 0;
	textureDescDiffuse.MiscFlags = 0;

	//Create render target array


	device->CreateTexture2D(&textureDescPosNorm, 0, &renderTargetTextureArray[0]);
	device->CreateTexture2D(&textureDescPosNorm, 0, &renderTargetTextureArray[1]);
	device->CreateTexture2D(&textureDescPosNorm, 0, &renderTargetTextureArray[2]);


	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc32;
	ZeroMemory(&renderTargetViewDesc32, sizeof(renderTargetViewDesc32));
	renderTargetViewDesc32.Format = textureDescPosNorm.Format;
	renderTargetViewDesc32.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc32.Texture2D.MipSlice = 0;

	// Render target desc for 8bit
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc8;
	ZeroMemory(&renderTargetViewDesc8, sizeof(renderTargetViewDesc8));
	renderTargetViewDesc8.Format = textureDescPosNorm.Format;
	renderTargetViewDesc8.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc8.Texture2D.MipSlice = 0;


	//Create the render target arrays
	device->CreateRenderTargetView(renderTargetTextureArray[0], &renderTargetViewDesc32, &renderTargetViewArray[0]);
	device->CreateRenderTargetView(renderTargetTextureArray[1], &renderTargetViewDesc32, &renderTargetViewArray[1]);
	device->CreateRenderTargetView(renderTargetTextureArray[2], &renderTargetViewDesc8, &renderTargetViewArray[2]);

	//Creste SRV desc
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescDR;
	ZeroMemory(&shaderResourceViewDescDR, sizeof(shaderResourceViewDescDR));
	shaderResourceViewDescDR.Format = textureDescPosNorm.Format;
	shaderResourceViewDescDR.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDescDR.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDescDR.Texture2D.MipLevels = 1;

	//Create SRV's
	for (i = 0; i < 3; i++)
	{
		device->CreateShaderResourceView(renderTargetTextureArray[i], &shaderResourceViewDescDR, &shaderResourceViewArray[i]);

	}

	//for (i = 0; i < 3; i++)
	//{
	//	renderTargetTextureArray[i]->Release();
	//}
	D3D11_DEPTH_STENCIL_DESC depthStencilDescDR = {};
	depthStencilDescDR.DepthEnable = true;
	depthStencilDescDR.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDescDR.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&depthStencilDescDR, &depthStateDR);

	return true;
}

bool Graphics::Initialize2dStuff(HWND hwnd)
{
	HRESULT hr;

	D2D1_FACTORY_OPTIONS options = {};
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, &pD2DFactory_);
	COM_ERROR_IF_FAILED(hr, "D2D1CreateFactory.");

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),&pDWriteFactory_);
	COM_ERROR_IF_FAILED(hr, "DWriteCreateFactory.");
	
	hr = this->device.As(&dxgiDevice);
	COM_ERROR_IF_FAILED(hr, "device.As.");

	hr = pD2DFactory_->CreateDevice(dxgiDevice.Get(), &m_d2dDevice);
	COM_ERROR_IF_FAILED(hr, "CreateDevice.");

	hr = m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dContext);
	COM_ERROR_IF_FAILED(hr, "CreateDeviceContext.");

	m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBlackBrush_);

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(gammaCorrection ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			1,
			1
		);

	Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
	hr = swapchain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	COM_ERROR_IF_FAILED(hr, "GetBuffer.");

	hr = m_d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(),&bitmapProperties,&m_d2dTargetBitmap);
	COM_ERROR_IF_FAILED(hr, "CreateBitmapFromDxgiSurface.");

	m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());

	m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	hr = pDWriteFactory_->CreateTextFormat(
		L"Gabriola",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		72.0f,
		L"en-us",
		&pTextFormat_
	);

	hr = pDWriteFactory_->CreateTextFormat(
		L"Comic Sans MS",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		36.0f,
		L"en-us",
		&pTextFormatSans_
	);

	return true;
}

void Graphics::renderDeferredGeometry()
{
	this->deviceContext->VSSetShader(deferredshaderVS.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(deferredshaderPS.GetShader(), NULL, 0);
	
	this->deviceContext->IASetInputLayout(this->deferredshaderVS.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->PSSetSamplers(1, 1, this->clampSamplerState.GetAddressOf());
	
	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->OMSetRenderTargets(3, renderTargetViewArray, depthStencilView.Get());


	deviceContext->ClearRenderTargetView(renderTargetViewArray[0], color);
	deviceContext->ClearRenderTargetView(renderTargetViewArray[1], color);
	deviceContext->ClearRenderTargetView(renderTargetViewArray[2], color);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH , 1.0f, 0);
	this->deviceContext->RSSetViewports(1, &viewport);

	for (auto i = 0; i < models.size(); i++)
	{


		models[i].DrawToDepth(camera.GetViewMatrix() * camera.GetProjectionMatrix(), light.GetViewMatrix() * light.GetProjectionMatrix());

	}


	
}

void Graphics::renderDeferredlightPass()
{
}
