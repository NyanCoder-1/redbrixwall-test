#include "SamplerState.h"
#include "Render.h"
#include <iostream>

std::shared_ptr<SamplerState> SamplerState::CreateDefaultSampler(Render* render)
{
	auto sampler = std::make_shared<SamplerState>(render);
	return sampler->InitDefault() ? sampler : nullptr;
}

std::shared_ptr<SamplerState> SamplerState::CreatePixelatedSampler(Render* render)
{
	auto sampler = std::make_shared<SamplerState>(render);
	return sampler->InitPixelated() ? sampler : nullptr;
}

void SamplerState::Draw()
{
	render->GetDeviceContext()->PSSetSamplers(0, 1, sampleState.GetAddressOf());
}

SamplerState::SamplerState(Render* render)
{
	this->render = render;
}

bool SamplerState::InitDefault()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = render->GetDevice()->CreateSamplerState(&samplerDesc, sampleState.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Не удалось создать sample state\n";
		return false;
	}

	return true;
}

bool SamplerState::InitPixelated()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = render->GetDevice()->CreateSamplerState(&samplerDesc, sampleState.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Не удалось создать sample state\n";
		return false;
	}

	return true;
}
