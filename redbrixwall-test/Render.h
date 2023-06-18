#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <cstdint>
#include <utility>
#include <memory>
#include "Shader.h"
#include "Texture.h"
#include "SamplerState.h"
#include "BMPFont.h"
#include "BMPText.h"
#include "FontsContainer.h"
#include <functional>


class FieldMesh;

class Text;
class MeshColored;

class Render
{
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Render();
	~Render();

	void Draw(std::function<void(Render* render)> drawGameLayer, std::function<void(Render* render)> drawUILayer);
	bool Init(HWND hWnd);

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;

	Shader* GetShaderColored() const;
	Shader* GetShaderTextured() const;
	const FontsContainer* GetFontsContainer() const;
	const DirectX::XMMATRIX GetMatrixUI() const;
	const DirectX::XMMATRIX GetMatrixViewProjection() const;

private:
	bool InitDeviceAndSwapChain();
	bool InitRenderTarget();
	bool InitBlendState();
	bool InitDepthStencil();
	void InitViewPort();
	void InitShaders();
	void InitRasterizerState();
	void InitGeometry();
	const DirectX::XMMATRIX CreateIsometricMatrix(int32_t screenWidth, int32_t screenHeight, float viewWidth, float viewHeight, float minDistance, float maxDistance) const;
	std::pair<int32_t, int32_t> GetWindowSize();

	HWND hWnd;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> ctx;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11BlendState> blendState;
	ComPtr<ID3D11RasterizerState> rasterizerState;
	ComPtr<ID3D11Texture2D> depthStencilTexture;
	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11DepthStencilState> depthStencilState;
	ComPtr<ID3D11DepthStencilState> depthDisabledStencilState;

	std::unique_ptr<FontsContainer> fontsContainer;
	std::shared_ptr<Shader> shaderTextured;
	std::shared_ptr<Shader> shaderColored;
	std::shared_ptr<SamplerState> samplerState;
	DirectX::XMMATRIX matrixOrtho;
	DirectX::XMMATRIX matrixIsometric;
};