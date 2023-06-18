#include "BMPFont.h"
#include <sstream>
#include <algorithm>

BMPFont::BMPFont(const void *data, size_t size)
{
	std::istringstream strIn(std::string(reinterpret_cast<const char*>(data), size), std::ios::in | std::ios::binary);
	uint32_t version = readUInt32(strIn);
	if (version == 0)
	{
		for (uint8_t width = readUInt8(strIn); strIn; width = readUInt8(strIn))
		{
			uint8_t height = readUInt8(strIn);
			uint32_t count = readUInt32(strIn);
			int16_t ascent = readUInt8(strIn);
			std::vector<uint32_t> chars = readGivenAmountOfUTF8Characters(strIn, count);
			uint32_t pixelsCount = width * height;
			uint32_t bytesPerGlyph = (7 + pixelsCount) / 8;
			for (uint32_t i = 0; i < count; ++i)
			{
				auto currentChar = chars[i];
				std::vector<uint8_t> glyphSrc(bytesPerGlyph);
				for (uint32_t j = 0; j < bytesPerGlyph; j++)
					glyphSrc[j] = readUInt8(strIn);
				uint8_t maxX = 0;
				uint8_t minX = width - 1;
				uint8_t maxY = 0;
				uint8_t minY = height - 1;
				bool haveBit = false;
				for (uint8_t y = 0; y < height; y++)
				{
					for (uint8_t x = 0; x < width; x++)
					{
						uint32_t index = x + y * width;
						if (glyphSrc[index / 8] & (1 << (7 - index % 8)))
						{
							minX = std::min(minX, x);
							maxX = std::max(maxX, x);
							minY = std::min(minY, y);
							maxY = std::max(maxY, y);
							haveBit = true;
						}
					}
				}
				if (haveBit)
				{
					uint8_t newWidth = maxX + 1 - minX;
					uint8_t newHeight = maxY + 1 - minY;
					int16_t newAscent = ascent - minY;
					uint32_t newPixelsCount = newWidth * newHeight;
					uint32_t bytesOfGlyph = (7 + newPixelsCount) / 8;
					std::vector<uint8_t> actualGlyph(bytesOfGlyph);
					for (uint8_t y = minY; y <= maxY; y++)
					{
						for (uint8_t x = minX; x <= maxX; x++)
						{
							uint32_t srcIndex = x + y * width;
							uint8_t destX = x - minX;
							uint8_t destY = y - minY;
							uint32_t destIndex = destX + destY * newWidth;
							if (glyphSrc[srcIndex / 8] & (1 << (7 - srcIndex % 8)))
								actualGlyph[destIndex / 8] |= (1 << (7 - destIndex % 8));
						}
					}
					characters[chars[i]] = TCharacter{ newWidth, newHeight, newAscent, actualGlyph };
				}
				else
				{
					characters[chars[i]] = TCharacter{ width, height, ascent };
				}
			}
		}
	}
}

const BMPFont::TCharacter *BMPFont::getCharacter(uint32_t character) const
{
	auto iter = characters.find(character);
	return iter != characters.end() ? &iter->second : nullptr;
}

std::vector<uint32_t> BMPFont::readGivenAmountOfUTF8Characters(std::istream &inStream, uint32_t charCount)
{
	std::vector<uint32_t> symbols;
	symbols.reserve(charCount);

	for (uint32_t i = 0; i < charCount; ++i)
	{
		uint8_t byte1 = inStream.get();
		if ((byte1 & 0x80) == 0) // 1-byte sequence
		{
			symbols.push_back(byte1);
		}
		else
		{
			uint8_t byte2 = inStream.get();

			if ((byte1 & 0xE0) == 0xC0) // 2-byte sequence
			{
				symbols.push_back(((byte1 & 0x1F) << 6) | (byte2 & 0x3F));
			}
			else if ((byte1 & 0xF0) == 0xE0) // 3-byte sequence
			{
				uint8_t byte3 = inStream.get();
				symbols.push_back(((byte1 & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F));
			}
			else if ((byte1 & 0xF8) == 0xF0) // 4-byte sequence
			{
				uint8_t byte3 = inStream.get();
				uint8_t byte4 = inStream.get();
				symbols.push_back(((byte1 & 0x07) << 18) | ((byte2 & 0x3F) << 12) | ((byte3 & 0x3F) << 6) | (byte4 & 0x3F));
			}
			else if ((byte1 & 0xFC) == 0xF8) // 5-byte sequence
			{
				uint8_t byte3 = inStream.get();
				uint8_t byte4 = inStream.get();
				uint8_t byte5 = inStream.get();
				symbols.push_back(((byte1 & 0x03) << 24) | ((byte2 & 0x3F) << 18) | ((byte3 & 0x3F) << 12) | ((byte4 & 0x3F) << 6) | (byte5 & 0x3F));
			}
			else if ((byte1 & 0xFE) == 0xFC) // 6-byte sequence
			{
				uint8_t byte3 = inStream.get();
				uint8_t byte4 = inStream.get();
				uint8_t byte5 = inStream.get();
				uint8_t byte6 = inStream.get();
				symbols.push_back(((byte1 & 0x01) << 30) | ((byte2 & 0x3F) << 24) | ((byte3 & 0x3F) << 18) | ((byte4 & 0x3F) << 12) | ((byte5 & 0x3F) << 6) | (byte6 & 0x3F));
			}
			else
			{
				// Invalid UTF-8 sequence
				throw Exception::InvalidUTF8Sequence();
			}
		}
	}

	return symbols;
}

uint32_t BMPFont::bitsToBytes(uint32_t bitsCount)
{
	return bitsCount / 8 + (((bitsCount % 8) > 0) ? 1 : 0);
}

uint8_t BMPFont::readUInt8(std::istream &inStream)
{
	uint8_t value = inStream.get();

	return value;
}

uint16_t BMPFont::readUInt16(std::istream &inStream)
{
	uint8_t byte1 = inStream.get();
	uint8_t byte2 = inStream.get();

	uint16_t value = (static_cast<uint16_t>(byte1) << 8) |
					 static_cast<uint16_t>(byte2);

	return value;
}

uint32_t BMPFont::readUInt32(std::istream &inStream)
{
	uint8_t byte1 = inStream.get();
	uint8_t byte2 = inStream.get();
	uint8_t byte3 = inStream.get();
	uint8_t byte4 = inStream.get();

	uint32_t value = (static_cast<uint32_t>(byte1) << 24) |
					 (static_cast<uint32_t>(byte2) << 16) |
					 (static_cast<uint32_t>(byte3) << 8) |
					 static_cast<uint32_t>(byte4);

	return value;
}

uint8_t BMPFont::readUInt8(const void *ptr)
{
	const uint8_t* bytes = reinterpret_cast<const uint8_t*>(ptr);

	return *bytes;
}

uint16_t BMPFont::readUInt16(const void *ptr)
{
	const uint8_t* bytes = reinterpret_cast<const uint8_t*>(ptr);

	uint16_t value = (static_cast<uint16_t>(bytes[0]) << 8) |
					 static_cast<uint16_t>(bytes[1]);

	return value;
}

uint32_t BMPFont::readUInt32(const void *ptr)
{
	const uint8_t* bytes = reinterpret_cast<const uint8_t*>(ptr);

	uint32_t value = (static_cast<uint32_t>(bytes[0]) << 24) |
					 (static_cast<uint32_t>(bytes[1]) << 16) |
					 (static_cast<uint32_t>(bytes[2]) << 8) |
					 static_cast<uint32_t>(bytes[3]);

	return value;
}

void BMPFont::movePtr(const void *&ptr, uintptr_t delta)
{
	ptr = reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(ptr) + delta);
}

bool BMPFont::eof(const void *dataPtr, const void *currentPtr, size_t size)
{
	return (reinterpret_cast<uintptr_t>(dataPtr) + size) <= reinterpret_cast<uintptr_t>(currentPtr);
}
