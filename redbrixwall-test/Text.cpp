#include "Text.h"
#include "Render.h"
#include "FontsContainer.h"
#include "Buffer.h"
#include <DirectXMath.h>
#include "Texture.h"

Text::Text(Render* render, std::u32string string, bool useBufferedCharCollection) :
	DrawablePrimitive(render), indicesCount(0)
{
	textGenerator = std::make_unique<BMPText>(std::vector<const BMPFont*>{ render->GetFontsContainer()->GetMinecraft(), render->GetFontsContainer()->GetUnifont() });
	textGenerator->setSizeMultiplier(render->GetFontsContainer()->GetUnifont(), 2.0f);
	textGenerator->setSizeMultiplier(render->GetFontsContainer()->GetMinecraft(), 3.0f);
	if (useBufferedCharCollection)
	{
		charCollection = textGenerator->prepareAtlas(string);
		auto textMesh = textGenerator->createTextFromAtlas(charCollection.value(), string);
		textVertexBuffer = Buffer::CreateVertexBuffer(render->GetDevice(), static_cast<int>(sizeof(TVertexTextured) * textMesh.vertices.size()), false, textMesh.vertices.data());
		textIndexBuffer = Buffer::CreateIndexBuffer(render->GetDevice(), static_cast<int>(sizeof(uint32_t) * textMesh.indices.size()), false, textMesh.indices.data());
		indicesCount = static_cast<int32_t>(textMesh.indices.size());

		textTexture = Texture::CreateTextureFromMemory(render, reinterpret_cast<uint8_t*>(charCollection.value().textureData.data()), charCollection.value().width, charCollection.value().height);
	}
	else
	{
		auto textMesh = textGenerator->createText(string);
		textVertexBuffer = Buffer::CreateVertexBuffer(render->GetDevice(), static_cast<int>(sizeof(TVertexTextured) * textMesh.vertices.size()), false, textMesh.vertices.data());
		textIndexBuffer = Buffer::CreateIndexBuffer(render->GetDevice(), static_cast<int>(sizeof(uint32_t) * textMesh.indices.size()), false, textMesh.indices.data());
		indicesCount = static_cast<int32_t>(textMesh.indices.size());

		textTexture = Texture::CreateTextureFromMemory(render, reinterpret_cast<uint8_t*>(textMesh.textureData.data()), textMesh.textureWidth, textMesh.textureHeight);
	}
	constBuffer = Buffer::CreateConstantBuffer(render->GetDevice(), sizeof(DirectX::XMMATRIX) * 2, false);
	struct {
		DirectX::XMMATRIX mat;
		DirectX::XMMATRIX matWorld;
	} constBuff;
	constBuff.mat = render->GetMatrixUI();
	constBuff.matWorld = DirectX::XMMatrixIdentity();
	render->GetDeviceContext()->UpdateSubresource(constBuffer.Get(), 0, nullptr, &constBuff, 0, 0);
}

void Text::Draw()
{
	render->GetShaderTextured()->Draw();
	unsigned int stride = sizeof(TVertexTextured);
	unsigned int offset = 0;
	render->GetDeviceContext()->IASetVertexBuffers(0, 1, textVertexBuffer.GetAddressOf(), &stride, &offset);
	render->GetDeviceContext()->IASetIndexBuffer(textIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	render->GetDeviceContext()->VSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());
	render->GetDeviceContext()->PSSetShaderResources(0, 1, textTexture.GetAddressOf());
	render->GetDeviceContext()->DrawIndexed(indicesCount, 0, 0);
}

void Text::SetPosition(float x, float y)
{
	struct {
		DirectX::XMMATRIX mat;
		DirectX::XMMATRIX matWorld;
	} constBuff;
	constBuff.mat = DirectX::XMMatrixTranslation(x, y, 0) * render->GetMatrixUI();
	constBuff.matWorld = DirectX::XMMatrixTranslation(x, y, 0);
	render->GetDeviceContext()->UpdateSubresource(constBuffer.Get(), 0, nullptr, &constBuff, 0, 0);
}

void Text::SetText(std::u32string text)
{
	if (charCollection.has_value())
	{
		auto textMesh = textGenerator->createTextFromAtlas(charCollection.value(), text);
		textVertexBuffer = Buffer::CreateVertexBuffer(render->GetDevice(), static_cast<int>(sizeof(TVertexTextured) * textMesh.vertices.size()), false, textMesh.vertices.data());
		textIndexBuffer = Buffer::CreateIndexBuffer(render->GetDevice(), static_cast<int>(sizeof(uint32_t) * textMesh.indices.size()), false, textMesh.indices.data());
		indicesCount = static_cast<int32_t>(textMesh.indices.size());
	}
	else
	{
		auto textMesh = textGenerator->createText(text);
		textVertexBuffer = Buffer::CreateVertexBuffer(render->GetDevice(), static_cast<int>(sizeof(TVertexTextured) * textMesh.vertices.size()), false, textMesh.vertices.data());
		textIndexBuffer = Buffer::CreateIndexBuffer(render->GetDevice(), static_cast<int>(sizeof(uint32_t) * textMesh.indices.size()), false, textMesh.indices.data());
		indicesCount = static_cast<int32_t>(textMesh.indices.size());
		textTexture = Texture::CreateTextureFromMemory(render, reinterpret_cast<uint8_t*>(textMesh.textureData.data()), textMesh.textureWidth, textMesh.textureHeight);
	}
}
