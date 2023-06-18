#pragma once

#include "DrawablePrimitive.h"
#include <string>
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <optional>
#include "BMPText.h"

class Text : public DrawablePrimitive
{
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Text(Render* render, std::u32string string, bool useBufferedCharCollection = false);

	void Draw() override;

	void SetPosition(float x, float y);
	void SetText(std::u32string text);

private:
	ComPtr<ID3D11Buffer> textVertexBuffer;
	ComPtr<ID3D11Buffer> textIndexBuffer;
	ComPtr<ID3D11Buffer> constBuffer;
	ComPtr<ID3D11ShaderResourceView> textTexture;
	std::unique_ptr<BMPText> textGenerator;
	std::optional<BMPText::TAtlas> charCollection;
	uint32_t indicesCount;
};
