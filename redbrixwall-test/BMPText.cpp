#include "BMPText.h"
#include "BMPFont.h"
#include <set>

BMPText::BMPText(std::vector<const BMPFont *> fonts)
{
	this->fonts = fonts;
}

BMPText::TOutTextWithTexture BMPText::createText(const std::u32string& string)
{
	TAtlas atlas = prepareAtlas(string);
	auto outPre = createTextFromAtlas(atlas, string);
	BMPText::TOutTextWithTexture out;// = { .textureWidth = atlas.width, .textureHeight = atlas.height, .textureData = std::move(atlas.textureData) };
	out.vertices = std::move(outPre.vertices);
	out.indices = std::move(outPre.indices);
	out.textureWidth = atlas.width;
	out.textureHeight = atlas.height;
	out.textureData = std::move(atlas.textureData);
	return out;
}

BMPText::TOutText BMPText::createTextFromAtlas(const TAtlas &atlas, const std::u32string &string)
{
	TOutText out;
	float x = 0;
	uint32_t i = 0;
	for (size_t ch = 0; ch < string.size(); ch++)
	{
		uint32_t character = string[ch];

		auto charIter = atlas.characters.find(character);
		if (charIter == atlas.characters.end())
			continue;
		// space
		if (charIter->second.space)
		{
			x += charIter->second.width + charIter->second.marginRight;
			continue;
		}
		out.vertices.emplace_back(x							, -charIter->second.ascent							, 0.0f, charIter->second.u1, charIter->second.v1);
		out.vertices.emplace_back(x + charIter->second.width, -charIter->second.ascent							, 0.0f, charIter->second.u2, charIter->second.v1);
		out.vertices.emplace_back(x							, charIter->second.height - charIter->second.ascent	, 0.0f, charIter->second.u1, charIter->second.v2);
		out.vertices.emplace_back(x + charIter->second.width, charIter->second.height - charIter->second.ascent	, 0.0f, charIter->second.u2, charIter->second.v2);
		x += charIter->second.width;

		// add margin only if the next symbol is not space
		if ((ch + 1) < string.size())
		{
			if (auto nextCharIter = atlas.characters.find(string[ch + 1]); nextCharIter != atlas.characters.end() && !nextCharIter->second.space)
				x += charIter->second.marginRight;
		}

		out.indices.emplace_back(i + 0);
		out.indices.emplace_back(i + 1);
		out.indices.emplace_back(i + 2);
		out.indices.emplace_back(i + 2);
		out.indices.emplace_back(i + 1);
		out.indices.emplace_back(i + 3);
		i += 4;
	}
    return out;
}

void BMPText::setSizeMultiplier(const BMPFont *font, float mul)
{
	customFontMultiplier[font] = mul;
}

float BMPText::getSizeMultiplier(const BMPFont *font) const
{
	auto iter = customFontMultiplier.find(font);
	return iter != customFontMultiplier.end() ? iter->second : 1.0f;
}

BMPText::TAtlas BMPText::prepareAtlas(const std::u32string &string)
{
	TAtlas atlas;

	// prepare texture size
	atlas.width = 0;
	atlas.height = 0;
	std::map<uint32_t, const BMPFont::TCharacter*> characters;
	for (const auto& character : string)
	{
		// skip if already in list
		if (characters.find(character) != characters.end())
			continue;
		
		// accumulate atlas size and list
		for (auto font : fonts)
		{
			if (!font)
				continue;
			auto glyph = font->getCharacter(character);
			if (!glyph)
				continue;
			atlas.width += glyph->width + 1;
			atlas.height = std::max<uint32_t>(atlas.height, glyph->height);
			characters[character] = glyph;
			auto mulIter = customFontMultiplier.find(font);
			float mul = mulIter != customFontMultiplier.end() ? mulIter->second : 1.0f;
			atlas.characters[character].width = glyph->width * mul;
			atlas.characters[character].height = glyph->height * mul;
			atlas.characters[character].ascent = glyph->ascent * mul;
			atlas.characters[character].marginRight = mul;
			break;
		}
	}
	atlas.width -= 1; // remove last space

	uint32_t atlasX = 0;
	atlas.textureData.resize(atlas.width * atlas.height);
	std::set<uint32_t> visitedCharacters;
	for (const auto& character : string)
	{
		if (visitedCharacters.count(character))
			continue;
		visitedCharacters.insert(character);
		auto charInfoIter = characters.find(character);
		if (charInfoIter == characters.end())
			continue;
		
		atlas.characters[character].u1 = float(atlasX) / atlas.width;
		atlas.characters[character].v1 = 0.0f;
		atlas.characters[character].u2 = float(charInfoIter->second->width) / atlas.width + atlas.characters[character].u1;
		atlas.characters[character].v2 = float(charInfoIter->second->height) / atlas.height;
		// skip if space
		if (charInfoIter->second->textureData.empty())
		{
			atlas.characters[character].space = true;
			continue;
		}
		// fill atlas
		union TColor {
			uint32_t data;
			struct { // I think this thing should help make this endiannes-agnostic
				uint8_t r, g, b, a;
			};
		} activeTextColor, inactiveTextColor;
		activeTextColor.r = 0xff;
		activeTextColor.g = 0xff;
		activeTextColor.b = 0xff;
		activeTextColor.a = 0xff;
		inactiveTextColor.r = 0xff;
		inactiveTextColor.g = 0xff;
		inactiveTextColor.b = 0xff;
		inactiveTextColor.a = 0x00;
		for (uint8_t y = 0; y < charInfoIter->second->height; y++)
		{
			for (uint8_t x = 0; x < charInfoIter->second->width; x++)
			{
				uint16_t index = x + y * charInfoIter->second->width;
				atlas.textureData[x + atlasX + y * atlas.width] = charInfoIter->second->textureData[index / 8] & (1 << (7 - index % 8)) ? activeTextColor.data : inactiveTextColor.data;
			}
		}
		atlasX += charInfoIter->second->width + 1;
	}

	return atlas;
}
