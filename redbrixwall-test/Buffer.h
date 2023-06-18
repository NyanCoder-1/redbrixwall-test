#pragma once

#include <d3d11.h>
#include <wrl.h>

namespace Buffer
{
	template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	ComPtr<ID3D11Buffer> CreateVertexBuffer(ID3D11Device* device, int size, bool dynamic, const void* Mem);
	ComPtr<ID3D11Buffer> CreateIndexBuffer(ID3D11Device* device, int size, bool dynamic, const void* Mem);
	ComPtr<ID3D11Buffer> CreateConstantBuffer(ID3D11Device* device, int size, bool dynamic);
}