#pragma once

#include "DrawablePrimitive.h"
#include <wrl.h>
#include <d3d11.h>
#include <cstdint>
#include <vector>
#include "types.h"

template <typename TVertex> class Mesh : public DrawablePrimitive
{
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Mesh(Render* render);

	void Draw() override;

protected:
	ComPtr<ID3D11Buffer> textVertexBuffer;
	ComPtr<ID3D11Buffer> textIndexBuffer;
	ComPtr<ID3D11Buffer> constBuffer;
	uint32_t indicesCount = 0;
};

class MeshColored : public Mesh<TVertexColored>
{
public:
	MeshColored(Render* render, std::vector<TVertexColored> vertices, std::vector<uint32_t> indices);

	void Draw() override;

protected:
};

class MeshTextured : public Mesh<TVertexTextured>
{
public:
	MeshTextured(Render* render, std::vector<TVertexTextured> vertices, std::vector<uint32_t> indices, ComPtr<ID3D11ShaderResourceView> texture);

	void Draw() override;

protected:
	ComPtr<ID3D11ShaderResourceView> texture;
};
