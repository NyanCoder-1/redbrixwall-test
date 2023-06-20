#include "Mesh.h"
#include "Render.h"
#include "Buffer.h"
#include <array>
#include <DirectXMath.h>
#include "Shader.h"

MeshBase::MeshBase(Render* render) :
	DrawablePrimitive(render)
{
}

void MeshBase::UpdateMatrixFor2D(DirectX::XMMATRIX matrix)
{
	struct {
		DirectX::XMMATRIX mat;
		DirectX::XMMATRIX matWorld;
	} constBuff;
	constBuff.mat = matrix * render->GetMatrixUI();
	constBuff.matWorld = matrix;
	render->GetDeviceContext()->UpdateSubresource(constBuffer.Get(), 0, nullptr, &constBuff, 0, 0);
}

void MeshBase::UpdateMatrixFor3D(DirectX::XMMATRIX matrix)
{
	struct {
		DirectX::XMMATRIX mat;
		DirectX::XMMATRIX matWorld;
	} constBuff;
	constBuff.mat = matrix * render->GetMatrixViewProjection();
	constBuff.matWorld = matrix;
	render->GetDeviceContext()->UpdateSubresource(constBuffer.Get(), 0, nullptr, &constBuff, 0, 0);
}

template<typename TVertex>
inline Mesh<TVertex>::Mesh(Render* render) :
	MeshBase(render)
{
	constBuffer = Buffer::CreateConstantBuffer(render->GetDevice(), sizeof(DirectX::XMMATRIX) * 2, false);
	UpdateMatrixFor3D();
}

template<typename TVertex>
inline void Mesh<TVertex>::Draw()
{
	unsigned int stride = sizeof(TVertex);
	unsigned int offset = 0;
	render->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	render->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	render->GetDeviceContext()->VSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());
	render->GetDeviceContext()->DrawIndexed(indicesCount, 0, 0);
}

template<typename TVertex>
void Mesh<TVertex>::UpdateGeometry(const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices)
{
	if (vertices.empty() || indices.empty())
	{
		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		indicesCount = 0;
		return;
	}
	vertexBuffer = Buffer::CreateVertexBuffer(render->GetDevice(), static_cast<int>(sizeof(TVertex) * vertices.size()), false, vertices.data());
	indexBuffer = Buffer::CreateIndexBuffer(render->GetDevice(), static_cast<int>(sizeof(uint32_t) * indices.size()), false, indices.data());
	indicesCount = static_cast<int32_t>(indices.size());
}

MeshColored::MeshColored(Render* render, const std::vector<TVertexColored>& vertices, const std::vector<uint32_t>& indices) :
	Mesh(render)
{
	UpdateGeometry(vertices, indices);
}

void MeshColored::Draw()
{
	render->GetShaderColored()->Draw();
	Mesh::Draw();
}

MeshTextured::MeshTextured(Render* render, const std::vector<TVertexTextured>& vertices, const std::vector<uint32_t>& indices, ComPtr<ID3D11ShaderResourceView> texture) :
	Mesh(render), texture(texture)
{
	UpdateGeometry(vertices, indices);
}

void MeshTextured::Draw()
{
	render->GetShaderTextured()->Draw();
	render->GetDeviceContext()->PSSetShaderResources(0, 1, texture.GetAddressOf());
	Mesh::Draw();
}

void MeshTextured::SetTexture(ComPtr<ID3D11ShaderResourceView> texture)
{
	this->texture = texture;
}
