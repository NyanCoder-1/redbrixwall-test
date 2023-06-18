#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <cstdint>

class Render;

namespace Texture
{
	template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// @brief Currently supports only 32bit rgba bitmaps
	ComPtr<ID3D11ShaderResourceView> CreateTextureFromMemory(Render* render, uint8_t* data, uint32_t width, uint32_t height);
};