#include "Buffer.h"

Buffer::ComPtr<ID3D11Buffer> Buffer::CreateVertexBuffer(ID3D11Device* device, int size, bool dynamic, const void* Mem)
{
	ComPtr<ID3D11Buffer> buffer;

	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.ByteWidth = size;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;
	if (dynamic)
	{
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.CPUAccessFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = Mem;
	Data.SysMemPitch = 0;
	Data.SysMemSlicePitch = 0;
	HRESULT hr = device->CreateBuffer(&BufferDesc, &Data, buffer.GetAddressOf());
	if (FAILED(hr))
		return nullptr;

	return buffer;
}

Buffer::ComPtr<ID3D11Buffer> Buffer::CreateIndexBuffer(ID3D11Device* device, int size, bool dynamic, const void* Mem)
{
	ComPtr<ID3D11Buffer> buffer;

	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.ByteWidth = size;
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;
	if (dynamic)
	{
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.CPUAccessFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = Mem;
	Data.SysMemPitch = 0;
	Data.SysMemSlicePitch = 0;
	HRESULT hr = device->CreateBuffer(&BufferDesc, &Data, buffer.GetAddressOf());
	if (FAILED(hr))
		return nullptr;

	return buffer;
}

Buffer::ComPtr<ID3D11Buffer> Buffer::CreateConstantBuffer(ID3D11Device* device, int size, bool dynamic)
{
	ComPtr<ID3D11Buffer> buffer;

	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.ByteWidth = size;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;
	if (dynamic)
	{
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.CPUAccessFlags = 0;
	}

	HRESULT hr = device->CreateBuffer(&BufferDesc, NULL, buffer.GetAddressOf());
	if (FAILED(hr))
		return nullptr;

	return buffer;
}
