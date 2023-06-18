#include "Texture.h"
#include "Render.h"

Texture::ComPtr<ID3D11ShaderResourceView> Texture::CreateTextureFromMemory(Render* render, uint8_t* data, uint32_t width, uint32_t height)
{
	bool ret = false;

	ID3D11Device* device = render->GetDevice();
	ComPtr<ID3D11Texture2D> textureResource;
	ComPtr<ID3D11ShaderResourceView> resourceView;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (data)
	{
		D3D11_SUBRESOURCE_DATA subres;
		subres.pSysMem = data;
		subres.SysMemPitch = width * sizeof(uint32_t);
		subres.SysMemSlicePitch = 0; // Not needed since this is a 2d texture

		if (FAILED(device->CreateTexture2D(&textureDesc, &subres, textureResource.GetAddressOf())))
			return nullptr;
	}
	else
	{
		if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, textureResource.GetAddressOf())))
			return nullptr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	if (FAILED(device->CreateShaderResourceView(textureResource.Get(), &shaderResourceViewDesc, resourceView.GetAddressOf())))
		return nullptr;

	return resourceView;
}
