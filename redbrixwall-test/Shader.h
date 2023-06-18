#pragma once

#include <d3d11.h>
#include <vector>
#include <wrl.h>

class Render;

class Shader
{
public:
	template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Shader(Render* render);
	void AddInputElementDesc(const char* SemanticName, DXGI_FORMAT format);

	bool CreateShader(wchar_t* namevs, wchar_t* nameps);

	void Draw();
	void Close();

private:
	HRESULT compileshaderfromfile(WCHAR* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut);

	Render* render;

	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11InputLayout> layout;

	std::vector<D3D11_INPUT_ELEMENT_DESC> layoutformat;
};
