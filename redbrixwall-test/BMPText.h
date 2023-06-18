#pragma once

#include "types.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

class BMPFont;

class BMPText
{
public:
	struct TGlyphInfo
	{
		union
		{
			float uv[4];
			struct
			{
				float u1, v1;
				float u2, v2;
			};
		};
		float width;
		float height;
		float ascent;
		float marginRight;
		bool space = false;
	};

	struct TAtlas
	{
		uint32_t width;
		uint32_t height;
		std::vector<uint32_t> textureData;
		std::unordered_map<uint32_t, TGlyphInfo> characters;
	};

	struct TOutText
	{
		std::vector<TVertexTextured> vertices;
		std::vector<uint32_t> indices;
	};
	struct TOutTextWithTexture : public TOutText
	{
		uint32_t textureWidth;
		uint32_t textureHeight;
		std::vector<uint32_t> textureData;
		ePixelFormat pixelFormat = ePixelFormat::rgba;
	};
	struct TOutTextTexture
	{
		uint32_t textureWidth;
		uint32_t textureHeight;
		std::vector<uint32_t> textureData;
		std::vector<TVertexTextured> vertices;
		std::vector<uint32_t> indices;
		ePixelFormat pixelFormat = ePixelFormat::rgba;
	};

public:
	BMPText(std::vector<const BMPFont *> fonts);

	TOutTextWithTexture createText(const std::u32string& string);
	TOutText createTextFromAtlas(const TAtlas& atlas, const std::u32string& string);

	void setSizeMultiplier(const BMPFont *font, float mul);
	float getSizeMultiplier(const BMPFont *font) const;

	TAtlas prepareAtlas(const std::u32string& string);

private:
	std::vector<const BMPFont *> fonts;
	std::map<const BMPFont *, float> customFontMultiplier;
};