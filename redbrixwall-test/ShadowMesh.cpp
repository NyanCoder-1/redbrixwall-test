#include "ShadowMesh.h"
#include "Render.h"
#include "Shader.h"

ShadowMesh::ShadowMesh(Render* render) :
	MeshTextured(render,
		{ {-1.0f, -1.0f, 0.001f, -1.0f, -1.0f}, {-1.0f, 1.0f, 0.001f, -1.0f, 1.0f}, {1.0f, -1.0f, 0.001f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.001f, 1.0f, 1.0f} },
		{0, 1, 2, 2, 1, 3}, nullptr)
{
}

void ShadowMesh::Draw()
{
	render->GetShaderShadow()->Draw();
	Mesh::Draw();
}

void ShadowMesh::SetPosition(float x, float y)
{
	UpdateMatrixFor3D(DirectX::XMMatrixTranslation(x, y, 0.0f));
}
