#pragma once

#include <memory>
#include <wrl.h>
#include <d3d11.h>

class Render;

class SamplerState
{
public:
	template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	SamplerState(Render* render);

	static std::shared_ptr<SamplerState> CreateDefaultSampler(Render* render);
	static std::shared_ptr<SamplerState> CreatePixelatedSampler(Render* render);

	void Draw();

private:
	bool InitDefault();
	bool InitPixelated();

	Render* render;
	ComPtr<ID3D11SamplerState> sampleState;
};
