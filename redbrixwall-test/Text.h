#pragma once

#include "Mesh.h"
#include <string>
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <optional>
#include "BMPText.h"
#include <vector>

class Text : public MeshTextured
{
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Text(Render* render, std::u32string string, bool useBufferedCharCollection = false);

	void Draw() override;

	void SetPosition(float x, float y);
	void SetText(std::u32string text);
	
	float GetMinX() const;
	float GetMaxX() const;
	float GetMinY() const;
	float GetMaxY() const;
	float GetWidth() const;
	float GetHeight() const;

private:
	static void UpdateBoundaries(const std::vector<TVertexTextured>& vertices, float& minX, float& maxX, float& minY, float& maxY);

	std::unique_ptr<BMPText> textGenerator;
	std::optional<BMPText::TAtlas> charCollection;
	uint32_t indicesCount;
	float minX;
	float maxX;
	float minY;
	float maxY;
};
