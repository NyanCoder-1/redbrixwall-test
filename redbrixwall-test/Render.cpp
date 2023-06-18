#include "render.h"
#include <array>
#include "Buffer.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "Text.h"
#include "Mesh.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

Render::Render()
{
	hWnd = nullptr;
}

Render::~Render()
{
	if (ctx.Get())
		ctx->ClearState();
}

void Render::Draw(std::function<void(Render* render)> drawGameLayer, std::function<void(Render* render)> drawUILayer)
{
	ctx->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	std::array<float, 4> ClearColor = { 0.404f, 0.38f, 0.349f, 1.0f };
	ctx->ClearRenderTargetView(renderTargetView.Get(), ClearColor.data());
	ctx->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	std::array<float, 4> blendFactor = {0.0f, 0.0f, 0.0f, 0.0f};
	ctx->OMSetBlendState(blendState.Get(), blendFactor.data(), 0xffffffff);

	samplerState->Draw();

	ctx->RSSetState(rasterizerState.Get());
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->OMSetDepthStencilState(depthStencilState.Get(), 0);
	drawGameLayer(this);

	ctx->OMSetDepthStencilState(depthDisabledStencilState.Get(), 0);
	drawUILayer(this);

	swapChain->Present(0, 0);
}

bool Render::Init(HWND hWnd)
{
	this->hWnd = hWnd;
	if (!InitDeviceAndSwapChain() || !InitRenderTarget() || !InitBlendState() || !InitDepthStencil())
		return false;
	InitViewPort();
	InitShaders();
	InitRasterizerState();
	InitGeometry();
	return true;
}

ID3D11Device* Render::GetDevice() const
{
	return device.Get();
}

ID3D11DeviceContext* Render::GetDeviceContext() const
{
	return ctx.Get();
}

Shader* Render::GetShaderColored() const
{
	return shaderColored.get();
}

Shader* Render::GetShaderTextured() const
{
	return shaderTextured.get();
}

const FontsContainer* Render::GetFontsContainer() const
{
	return fontsContainer.get();
}

const DirectX::XMMATRIX Render::GetMatrixUI() const
{
	return matrixOrtho;
}

const DirectX::XMMATRIX Render::GetMatrixViewProjection() const
{
	return matrixIsometric;
}

bool Render::InitDeviceAndSwapChain()
{
	auto wndSize = GetWindowSize();
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	std::array<D3D_DRIVER_TYPE, 3> driverTypes = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	std::array<D3D_FEATURE_LEVEL, 2> featureLevels = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	DXGI_SWAP_CHAIN_DESC sd;
	memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = wndSize.first;
	sd.BufferDesc.Height = wndSize.second;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 75;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	for (auto driverType : driverTypes)
	{
		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, createDeviceFlags, featureLevels.data(), static_cast<UINT>(featureLevels.size()), D3D11_SDK_VERSION, &sd, swapChain.GetAddressOf(), device.GetAddressOf(), &featureLevel, ctx.GetAddressOf());
		if (SUCCEEDED(hr))
			return true;
	}

	return false;
}

bool Render::InitRenderTarget()
{
	ID3D11Texture2D* pBackBuffer = NULL;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr)) return false;
	hr = device->CreateRenderTargetView(pBackBuffer, NULL, renderTargetView.GetAddressOf());
	pBackBuffer->Release();
	if (FAILED(hr)) return false;
	ctx->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), NULL);
	return true;
}

bool Render::InitBlendState()
{
	D3D11_BLEND_DESC blendStateDescription;
	memset(&blendStateDescription, 0, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	HRESULT hr = device->CreateBlendState(&blendStateDescription, blendState.GetAddressOf());
	return SUCCEEDED(hr);
}

bool Render::InitDepthStencil()
{
	auto [width, height] = GetWindowSize();
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	HRESULT hr = device->CreateTexture2D(&descDepth, NULL, depthStencilTexture.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	if (FAILED(hr))
		return false;

	depthStencilDesc.DepthEnable = false;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthDisabledStencilState.GetAddressOf());
	if (FAILED(hr))
		return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depthStencilTexture.Get(), &descDSV, depthStencilView.GetAddressOf());
	if (FAILED(hr))
		return false;

	ctx->OMSetDepthStencilState(depthStencilState.Get(), 0);

	return true;
}

void Render::InitViewPort()
{
	auto wndSize = GetWindowSize();
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(wndSize.first);
	vp.Height = static_cast<FLOAT>(wndSize.second);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	ctx->RSSetViewports(1, &vp);
}

void Render::InitShaders()
{
	shaderTextured = std::make_shared<Shader>(this);
	shaderTextured->AddInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	shaderTextured->AddInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	shaderTextured->CreateShader(const_cast<wchar_t*>(L".\\assets\\shaders\\textured-vs.hlsl"), const_cast<wchar_t*>(L".\\assets\\shaders\\textured-ps.hlsl"));
	
	shaderColored = std::make_shared<Shader>(this);
	shaderColored->AddInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	shaderColored->AddInputElementDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	shaderColored->CreateShader(const_cast<wchar_t*>(L".\\assets\\shaders\\colored-vs.hlsl"), const_cast<wchar_t*>(L".\\assets\\shaders\\colored-ps.hlsl"));
}

void Render::InitRasterizerState()
{
	// Disable culling
	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	// create new rasterizer state
	device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
}

namespace {
	std::vector<uint8_t> readFile(const char* filename)
	{
		// open the file:
		std::streampos fileSize;
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			std::cout << "Failed to open the file " << filename << std::endl;
			return {};
		}

		// get its size:
		fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// read the data:
		std::vector<uint8_t> fileData(fileSize, 0);
		file.read((char*)fileData.data(), fileSize);
		return fileData;
	}
};

void Render::InitGeometry()
{
	fontsContainer = std::make_unique<FontsContainer>();
	fontsContainer->Init();

	auto size = GetWindowSize();
	matrixOrtho = DirectX::XMMatrixOrthographicOffCenterRH(0.0f, static_cast<float>(size.first), static_cast<float>(size.second), 0.0f, 0.0f, 100.0f);
	matrixIsometric = CreateIsometricMatrix(size.first, size.second, 200.0f * std::cosf(DirectX::XM_PI / 6), 101.0f, 0.0f, 100.0f / std::sin(DirectX::XM_PI / 4));

	samplerState = SamplerState::CreatePixelatedSampler(this);
}

const DirectX::XMMATRIX Render::CreateIsometricMatrix(int32_t screenWidth, int32_t screenHeight, float viewWidth, float viewHeight, float minDistance, float maxDistance) const
{
	float deltaDistance = maxDistance - minDistance;
	float screenRatio = float(screenWidth) / screenHeight;
	float viewRatio = viewWidth / viewHeight;
	float mulX = 1;
	float mulY = 1;
	float shiftY = -1;
	if (screenRatio > 1)
	{
		mulX /= screenRatio;
		if (screenRatio < viewRatio)
		{
			mulX *= (screenRatio / viewRatio);
			mulY *= (screenRatio / viewRatio);
		}
		else
		{
			mulX *= viewRatio;
			mulY *= viewRatio;
		}
	}
	else
	{
		mulY *= screenRatio;
		if (screenRatio > viewRatio)
		{
			mulX /= (screenRatio / viewRatio);
			mulY /= (screenRatio / viewRatio);
		}
		else
		{
			mulX /= viewRatio;
			mulY /= viewRatio;
		}
	}
	if (viewRatio < screenRatio)
	{
		mulX /= viewWidth / 2;
		mulY /= viewWidth / 2;
	}
	else
	{
		mulX /= viewHeight / 2;
		mulY /= viewHeight / 2;
	}
	return DirectX::XMMATRIX(
		std::cosf(DirectX::XM_PI / 6) * mulX, std::sin(DirectX::XM_PI / 6) * mulY, std::sin(DirectX::XM_PI / 4) / deltaDistance, 0.0f,
		-std::cosf(DirectX::XM_PI / 6) * mulX, std::sin(DirectX::XM_PI / 6) * mulY, std::sin(DirectX::XM_PI / 4) / deltaDistance, 0.0f,
		0.0f, mulY, 0.0f, 0.0f,
		0.0f, shiftY, -minDistance / deltaDistance, 1.0f
	);
}

std::pair<int32_t, int32_t> Render::GetWindowSize()
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	int32_t width = rc.right - rc.left;
	int32_t height = rc.bottom - rc.top;

	return { width, height };
}
