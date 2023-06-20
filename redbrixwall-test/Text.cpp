#include "Text.h"
#include "Render.h"
#include "FontsContainer.h"
#include "Buffer.h"
#include <DirectXMath.h>
#include "Texture.h"
#include "Shader.h"

Text::Text(Render* render, std::u32string string, bool useBufferedCharCollection) :
	MeshTextured(render, {}, {}, nullptr), indicesCount(0)
{
	minX = 0;
	maxX = 0;
	minY = 0;
	maxY = 0;
	textGenerator = std::make_unique<BMPText>(std::vector<const BMPFont*>{ render->GetFontsContainer()->GetMinecraft(), render->GetFontsContainer()->GetUnifont() });
	textGenerator->setSizeMultiplier(render->GetFontsContainer()->GetUnifont(), 2.0f);
	textGenerator->setSizeMultiplier(render->GetFontsContainer()->GetMinecraft(), 3.0f);
	if (useBufferedCharCollection)
	{
		charCollection = textGenerator->prepareAtlas(string);
		auto textMesh = textGenerator->createTextFromAtlas(charCollection.value(), string);
		UpdateBoundaries(textMesh.vertices, minX, maxX, minY, maxY);
		UpdateGeometry(textMesh.vertices, textMesh.indices);
		SetTexture(Texture::CreateTextureFromMemory(render, reinterpret_cast<uint8_t*>(charCollection.value().textureData.data()), charCollection.value().width, charCollection.value().height));
	}
	else
	{
		auto textMesh = textGenerator->createText(string);
		UpdateBoundaries(textMesh.vertices, minX, maxX, minY, maxY);
		UpdateGeometry(textMesh.vertices, textMesh.indices);
		SetTexture(Texture::CreateTextureFromMemory(render, reinterpret_cast<uint8_t*>(textMesh.textureData.data()), textMesh.textureWidth, textMesh.textureHeight));
	}
	UpdateMatrixFor2D();
}

void Text::Draw()
{
	render->GetShaderTextured()->Draw();
	render->GetDeviceContext()->VSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());
	MeshTextured::Draw();
}

void Text::SetPosition(float x, float y)
{
	UpdateMatrixFor2D(DirectX::XMMatrixTranslation(x, y, 0));
}

void Text::SetText(std::u32string text)
{
	if (charCollection.has_value())
	{
		auto textMesh = textGenerator->createTextFromAtlas(charCollection.value(), text);
		UpdateBoundaries(textMesh.vertices, minX, maxX, minY, maxY);
		UpdateGeometry(textMesh.vertices, textMesh.indices);
	}
	else
	{
		auto textMesh = textGenerator->createText(text);
		UpdateBoundaries(textMesh.vertices, minX, maxX, minY, maxY);
		UpdateGeometry(textMesh.vertices, textMesh.indices);
		SetTexture(Texture::CreateTextureFromMemory(render, reinterpret_cast<uint8_t*>(textMesh.textureData.data()), textMesh.textureWidth, textMesh.textureHeight));
	}
}

float Text::GetMinX() const
{
	return minX;
}

float Text::GetMaxX() const
{
	return maxX;
}

float Text::GetMinY() const
{
	return minY;
}

float Text::GetMaxY() const
{
	return maxY;
}

float Text::GetWidth() const
{
	return maxX - minX;
}

float Text::GetHeight() const
{
	return maxY - minY;
}

void Text::UpdateBoundaries(const std::vector<TVertexTextured>& vertices, float& minX, float& maxX, float& minY, float& maxY)
{
	if (!vertices.empty())
	{
		minX = maxX = vertices.front().x;
		minY = maxY = vertices.front().y;
	}
	for (const auto& vertex : vertices)
	{
		minX = std::fminf(vertex.x, minX);
		maxX = std::fmaxf(vertex.x, minX);
		minY = std::fminf(vertex.y, minY);
		maxY = std::fmaxf(vertex.y, minY);
	}
}
