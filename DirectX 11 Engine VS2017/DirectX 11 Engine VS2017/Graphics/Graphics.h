#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "AnimatedTexture.h"
#include "Camera.h"
#include "..\\Timer.h"
#include "Model.h"
#include "Light.h"
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	Camera camera;
	Model modelPlayer;
	Model modelCube;
	Model skybox;
	Light light;
	std::vector<Model> models;
	int catsIn = 0;
	void restoreTargets();
	void restoreTargetsLum();

private:
	const int shadowMapSize = 1024;
	const bool gammaCorrection = false;
	const wchar_t* wszText_;
	UINT32 cTextLength_;
	D2D1_RECT_F layoutRect;
	RECT rect;
	
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	bool Initialize2dStuff(HWND hwnd);
	

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	
	Microsoft::WRL::ComPtr<IDWriteFactory> pDWriteFactory_;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormat_;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormatSans_;
	Microsoft::WRL::ComPtr<ID2D1Factory1> pD2DFactory_;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_pBackBufferRT;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBlackBrush_;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> pTextLayout_;


	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	Microsoft::WRL::ComPtr<ID2D1Device> m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_d2dContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_d2dTargetBitmap;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	

	Microsoft::WRL::ComPtr<ID3D11Texture2D> RTTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV;
	ID3D11ShaderResourceView* SRV;
	
	
	Microsoft::WRL::ComPtr<IDXGISurface> pBackBuffer;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> depthRenderTargetView;
	VertexShader depthvertexshader;
	PixelShader depthpixelshader;
	VertexShader vertexshader;
	PixelShader pixelshader;
	PixelShader skyshaderPS;
	VertexShader skyshaderVS;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_lightBuffer> cb_ps_lightBuffer;
	CD3D11_VIEWPORT viewport;

	/*Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowMapTex;*/
	

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTex;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;
	std::unique_ptr<AnimatedTexture> sprite;
	std::unique_ptr<AnimatedTexture> sprite1;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> clampSamplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyboxTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> catTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> playerTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> planeTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dog;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dog1;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	int windowWidth = 0;
	int windowHeight = 0;
	Timer fpsTimer;
	Timer deltaTimer;
};
