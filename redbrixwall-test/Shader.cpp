#include "Shader.h"
#include "Render.h"
#include <iostream>
#include <d3dcompiler.h>

Shader::Shader(Render* render)
{
	this->render = render;
}

void Shader::AddInputElementDesc(const char* SemanticName, DXGI_FORMAT format)
{
	D3D11_INPUT_ELEMENT_DESC layout;
	layout.SemanticName = SemanticName;
	layout.SemanticIndex = 0;
	layout.Format = format;
	layout.InputSlot = 0;
	layout.AlignedByteOffset = layoutformat.empty() ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
	layout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout.InstanceDataStepRate = 0;

	layoutformat.push_back(layout);
}

bool Shader::CreateShader(wchar_t* namevs, wchar_t* nameps)
{
	HRESULT hr = S_OK;
	ComPtr<ID3DBlob> vertexShaderBuffer;
	hr = compileshaderfromfile(namevs, "main", "vs_5_0", vertexShaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Не удалось загрузить вершинный шейдер ";
		std::wcerr << namevs << std::endl;
		return false;
	}

	ComPtr<ID3DBlob> pixelShaderBuffer;
	hr = compileshaderfromfile(nameps, "main", "ps_5_0", pixelShaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Не удалось загрузить пиксельный шейдер ";
		std::wcerr << nameps << std::endl;
		return false;
	}

	hr = render->GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, vertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Не удалось создать вершинный шейдер\n";
		return false;
	}

	hr = render->GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, pixelShader.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Не удалось создать пиксельный шейдер\n";
		return false;
	}

	hr = render->GetDevice()->CreateInputLayout(layoutformat.data(), static_cast<UINT>(layoutformat.size()), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), layout.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Не удалось создать формат ввода\n";
		return false;
	}

	return true;
}

void Shader::Draw()
{
	render->GetDeviceContext()->IASetInputLayout(layout.Get());
	render->GetDeviceContext()->VSSetShader(vertexShader.Get(), NULL, 0);
	render->GetDeviceContext()->PSSetShader(pixelShader.Get(), NULL, 0);
}

HRESULT Shader::compileshaderfromfile(WCHAR* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	ShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ComPtr<ID3DBlob> pErrorBlob;
	hr = D3DCompileFromFile(FileName, NULL, NULL, EntryPoint, ShaderModel, ShaderFlags, 0, ppBlobOut, pErrorBlob.GetAddressOf());
	if (FAILED(hr) && pErrorBlob.Get())
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

	return hr;
}
