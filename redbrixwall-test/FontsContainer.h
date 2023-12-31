#pragma once

#include <memory>

class BMPFont;

class FontsContainer
{
public:
	void Init();

	const BMPFont* GetUnifont() const;
	const BMPFont* GetMinecraft() const;

private:
	std::shared_ptr<BMPFont> fontUnifont;
	std::shared_ptr<BMPFont> fontMc;
};
