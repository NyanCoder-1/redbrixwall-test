#pragma once

#include "DrawablePrimitive.h"
#include <wrl.h>
#include <d3d11.h>
#include <cstdint>
#include <vector>
#include <DirectXMath.h>
#include "types.h"

class MeshBase : public DrawablePrimitive
{
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	MeshBase(Render* render);

	void UpdateMatrixFor2D(DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity());
	void UpdateMatrixFor3D(DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity());

protected:
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	ComPtr<ID3D11Buffer> constBuffer;
	uint32_t indicesCount = 0;
};

template <typename TVertex> class Mesh : public MeshBase
{
public:
	Mesh(Render* render);

	void Draw() override;

	void UpdateGeometry(const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices);
};

class MeshColored : public Mesh<TVertexColored>
{
public:
	MeshColored(Render* render, const std::vector<TVertexColored>& vertices, const std::vector<uint32_t>& indices);

	void Draw() override;
};

class MeshTextured : public Mesh<TVertexTextured>
{
public:
	MeshTextured(Render* render, const std::vector<TVertexTextured>& vertices, const std::vector<uint32_t>& indices, ComPtr<ID3D11ShaderResourceView> texture);

	void Draw() override;

	void SetTexture(ComPtr<ID3D11ShaderResourceView> texture);

protected:
	ComPtr<ID3D11ShaderResourceView> texture;
};
