#include "Mesh.h"
#include "Render.h"
#include "Buffer.h"
#include <array>
#include <DirectXMath.h>

template<typename TVertex>
inline Mesh<TVertex>::Mesh(Render* render) :
	DrawablePrimitive(render)
{
	constBuffer = Buffer::CreateConstantBuffer(render->GetDevice(), sizeof(DirectX::XMMATRIX) * 2, false);
	struct {
		DirectX::XMMATRIX mat;
		DirectX::XMMATRIX matWorld;
	} constBuff;
	constBuff.mat = render->GetMatrixViewProjection();
	constBuff.matWorld = DirectX::XMMatrixIdentity();
	render->GetDeviceContext()->UpdateSubresource(constBuffer.Get(), 0, nullptr, &constBuff, 0, 0);
}

template<typename TVertex>
inline void Mesh<TVertex>::Draw()
{
	unsigned int stride = sizeof(TVertex);
	unsigned int offset = 0;
	render->GetDeviceContext()->IASetVertexBuffers(0, 1, textVertexBuffer.GetAddressOf(), &stride, &offset);
	render->GetDeviceContext()->IASetIndexBuffer(textIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	render->GetDeviceContext()->VSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());
	render->GetDeviceContext()->DrawIndexed(indicesCount, 0, 0);
}

MeshColored::MeshColored(Render* render, std::vector<TVertexColored> vertices, std::vector<uint32_t> indices) :
	Mesh(render)
{
	textVertexBuffer = Buffer::CreateVertexBuffer(render->GetDevice(), static_cast<int>(sizeof(TVertexColored) * vertices.size()), false, vertices.data());
	textIndexBuffer = Buffer::CreateIndexBuffer(render->GetDevice(), static_cast<int>(sizeof(uint32_t) * indices.size()), false, indices.data());
	indicesCount = static_cast<int32_t>(indices.size());
}

void MeshColored::Draw()
{
	render->GetShaderColored()->Draw();
	Mesh::Draw();
}

MeshTextured::MeshTextured(Render* render, std::vector<TVertexTextured> vertices, std::vector<uint32_t> indices, ComPtr<ID3D11ShaderResourceView> texture) :
	Mesh(render), texture(texture)
{
	textVertexBuffer = Buffer::CreateVertexBuffer(render->GetDevice(), static_cast<int>(sizeof(TVertexTextured) * vertices.size()), false, vertices.data());
	textIndexBuffer = Buffer::CreateIndexBuffer(render->GetDevice(), static_cast<int>(sizeof(uint32_t) * indices.size()), false, indices.data());
	indicesCount = static_cast<int32_t>(indices.size());
}

void MeshTextured::Draw()
{
	render->GetShaderTextured()->Draw();
	render->GetDeviceContext()->PSSetShaderResources(0, 1, texture.GetAddressOf());
	Mesh::Draw();
}
