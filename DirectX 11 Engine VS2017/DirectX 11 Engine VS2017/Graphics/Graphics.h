#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "AnimatedTexture.h"
#include "Camera.h"
#include "..\\Timer.h"
#include "Model.h"
#include "Light.h"


class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	Camera camera;
	Model modelPlayer;
	Model modelCube;
	Light light;
	std::vector<Model> models;
	int catsIn = 0;
	void restoreTargets();

private:
	const int shadowMapSize = 1024;

	
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> depthRenderTargetView;
	VertexShader depthvertexshader;
	PixelShader depthpixelshader;
	VertexShader vertexshader;
	PixelShader pixelshader;
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
