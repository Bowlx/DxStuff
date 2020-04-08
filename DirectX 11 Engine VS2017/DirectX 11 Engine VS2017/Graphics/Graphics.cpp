#include "Graphics.h"
#include <random>

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->fpsTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;
	if (!light.Initialize(this->device.Get(), this->deviceContext.Get()))
		return false;
	return true;
}

void Graphics::RenderFrame()
{
	
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };


	this->deviceContext->VSSetShader(depthvertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(depthpixelshader.GetShader(), NULL, 0);
	this->deviceContext->IASetInputLayout(this->depthvertexshader.GetInputLayout());
	light.setRenderTarget(this->deviceContext.Get());
	light.ClearRenderTarget(this->deviceContext.Get());

	for (auto i = 0; i < models.size(); i++)
	{


		models[i].DrawToDepth(camera.GetViewMatrix() * camera.GetProjectionMatrix(),light.GetViewMatrix() * light.GetProjectionMatrix());

	}
	modelPlayer.DrawToDepth(camera.GetViewMatrix() * camera.GetProjectionMatrix(), light.GetViewMatrix() * light.GetProjectionMatrix());
	modelCube.DrawToDepth(camera.GetViewMatrix() * camera.GetProjectionMatrix(), light.GetViewMatrix() * light.GetProjectionMatrix());



	this->restoreTargets();
	
	light.setShaderResources(this->deviceContext.Get());
	this->restoreTargets();
	//
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	this->cb_ps_lightBuffer.data.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	this->cb_ps_lightBuffer.data.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	this->cb_ps_lightBuffer.ApplyChanges();
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_lightBuffer.GetAddressOf());
	for(auto i =0; i< models.size();i++)	
		{
	
			
			models[i].DrawToDepth(camera.GetViewMatrix() * camera.GetProjectionMatrix(), light.GetViewMatrix() * light.GetProjectionMatrix());
		
		}

		static XMFLOAT3  ground = XMFLOAT3(0.0f, -2.0f, 0);
		modelPlayer.SetPosition(camera.GetPositionVector() + camera.GetForwardVector()*6+ XMLoadFloat3(&ground));
		modelPlayer.DrawToDepth(camera.GetViewMatrix() * camera.GetProjectionMatrix(), light.GetViewMatrix() * light.GetProjectionMatrix());
		modelCube.DrawToDepth(camera.GetViewMatrix() * camera.GetProjectionMatrix(), light.GetViewMatrix() * light.GetProjectionMatrix());
	

	this->swapchain->Present(1, NULL);
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
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
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
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, //FOR SOFTWARE DRIVER TYPE
			NULL, //FLAGS FOR RUNTIME LAYERS
			NULL, //FEATURE LEVELS ARRAY
			0, //# OF FEATURE LEVELS IN ARRAY
			D3D11_SDK_VERSION,
			&scd, //Swapchain description
			this->swapchain.GetAddressOf(), //Swapchain Address
			this->device.GetAddressOf(), //Device Address
			NULL, //Supported feature level
			this->deviceContext.GetAddressOf()); //Device Context Address

		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");


		COM_ERROR_IF_FAILED(hr, "Failed to create render target buffer.");

		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		//Describe our Depth/Stencil Buffer
		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = this->device->CreateTexture2D(&depthStencilTextureDesc, NULL, this->depthStencilTex.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");
		
		hr = this->device->CreateDepthStencilView(this->depthStencilTex.Get(), nullptr, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");
	

		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		//Create & set the Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));;
		this->viewport = viewport;
		this->viewport.TopLeftX = 0.0f;
		this->viewport.TopLeftY = 0.0f;
		this->viewport.Width = static_cast<float>(this->windowWidth);
		this->viewport.Height = static_cast<float>(this->windowHeight);
		this->deviceContext->RSSetViewports(1, &this->viewport);

		//Create Rasterizer State
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Rasterizer State for culling front
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront, this->rasterizerState_CullFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Blend State
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC blendDesc = { 0 };
		blendDesc.RenderTarget[0] = rtbd;

		//hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		//Create sampler description for sampler state
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = this->device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

	
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		hr = this->device->CreateSamplerState(&samplerDesc, this->clampSamplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

		this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);
		this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		this->deviceContext->RSSetState(this->rasterizerState.Get());
		this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
		this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
		this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
		this->deviceContext->PSSetSamplers(1, 1, this->clampSamplerState.GetAddressOf());
		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
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
	{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};


	UINT numElements = ARRAYSIZE(layout);
	

	if (!vertexshader.Initialize(this->device, shaderfolder + L"vertexshader.cso", layout, numElements))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"pixelshader.cso"))
		return false;
	if (!depthvertexshader.Initialize(this->device, shaderfolder + L"depthVS.cso", layout, numElements))
		return false;

	if (!depthpixelshader.Initialize(this->device, shaderfolder + L"depthPS.cso"))
		return false;

	
	return true;
}
float get_random()
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(2, 8.5); // rage 0 - 1
	return dis(e);
}
bool Graphics::InitializeScene()
{
	srand(static_cast <unsigned> (time(0)));
	try
	{
		//Load Texture
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\cat.jpg", nullptr, catTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

		//Initialize Constant Buffer(s)
		hr = this->cb_vs_vertexshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

	/*	hr = this->cb_ps_pixelshader.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");*/
		hr = this->cb_ps_lightBuffer.Initialize(this->device.Get(), this->deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
		Model in;
		if (!in.Initialize("Data\\Objects\\cat.obj", this->device.Get(), this->deviceContext.Get(), this->catTexture.Get(), this->cb_vs_vertexshader))
			return false;
		for (auto j = 1; j <= 6; j++)
		{
			for (auto i = 0; i <= 10; i++)
			{
				static int la = 1;
				
				in.SetPosition( (get_random()* get_random()), 0.3, get_random()* get_random());
				in.SetScale(0.10 * j, 0.10 * j, 0.10 * j);
				in.SetRotation(0, get_random(), 0);
				models.push_back(in);

			}
		}

		 hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\seamless_grass.jpg", nullptr, playerTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");
		if (!modelPlayer.Initialize("Data\\Objects\\sphere.obj", this->device.Get(), this->deviceContext.Get(), this->playerTexture.Get(), this->cb_vs_vertexshader))
			return false;
		modelPlayer.SetPosition(0, -0.5  , 8);
		modelPlayer.SetScale(0.25, 0.25, 0.25);
		
		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\pinksquare.jpg", nullptr, planeTexture.GetAddressOf());
		if (!modelCube.Initialize("Data\\Objects\\cube.obj", this->device.Get(), this->deviceContext.Get(), this->planeTexture.Get(), this->cb_vs_vertexshader))
			return false;
		
		modelCube.SetScale(1000, 1, 10000);
		modelCube.AdjustPosition(-100, 0, 1000);
		camera.SetPosition(0.0f, 3.0f, 0.0f);
		camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);
		//light.SetPosition(15.0f, 25, 15.0f);
		light.SetProjectionValues(90, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 5, 100.0f);
		//light.SetLookAtPos(XMFLOAT3(16, 24, 15));
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}
