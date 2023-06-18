#pragma once

#include "types.h"
#include <vector>
#include <unordered_map>
#include <istream>

class BMPFont
{
public:
	struct TCharacter
	{
		uint8_t width;
		uint8_t height;
		int16_t ascent;
		std::vector<uint8_t> textureData;
		ePixelFormat pixelFormat = ePixelFormat::bits;
	};

public:
	BMPFont(const void* data, size_t size);

	const TCharacter* getCharacter(uint32_t character) const;

private:

	/// @brief this method reads given amount of utf-8 (1993) characters
	/// @param inStream input stream
	/// @param charCount amount of characters to read
	/// @return characters that have been read
	/// @throw Exception::InvalidUTF8Sequence (inherits std::runtime_error with "Invalid UTF-8 sequence" text)
	std::vector<uint32_t> readGivenAmountOfUTF8Characters(std::istream &inStream, uint32_t charCount);
	uint32_t bitsToBytes(uint32_t bitsCount);
	static uint8_t readUInt8(std::istream &inStream);
	static uint16_t readUInt16(std::istream &inStream);
	static uint32_t readUInt32(std::istream &inStream);
	static uint8_t readUInt8(const void* ptr);
	static uint16_t readUInt16(const void* ptr);
	static uint32_t readUInt32(const void* ptr);
	static void movePtr(const void*& ptr, uintptr_t delta);
	static bool eof(const void* dataPtr, const void* currentPtr, size_t size);

	std::unordered_map<uint32_t, TCharacter> characters;
};