#include "FontsContainer.h"
#include <fstream>
#include <iostream>
#include "BMPFont.h"

namespace {
	std::vector<uint8_t> readFile(const char* filename)
	{
		// open the file:
		std::streampos fileSize;
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			std::cerr << "Failed to open the file " << filename << std::endl;
			return {};
		}

		// get its size:
		fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// read the data:
		std::vector<uint8_t> fileData(fileSize, 0);
		file.read((char*)fileData.data(), fileSize);
		return fileData;
	}
};

void FontsContainer::Init()
{
	auto unifontData = readFile("./assets/fonts/unifont_all-15.0.05.bin");
	auto mcFontData = readFile("./assets/fonts/minecraftFont-default.bin");
	fontUnifont = std::make_shared<BMPFont>(unifontData.data(), unifontData.size());
	fontMc = std::make_shared<BMPFont>(mcFontData.data(), mcFontData.size());
}

const BMPFont* FontsContainer::GetUnifont() const
{
	return fontUnifont.get();
}

const BMPFont* FontsContainer::GetMinecraft() const
{
	return fontMc.get();
}
