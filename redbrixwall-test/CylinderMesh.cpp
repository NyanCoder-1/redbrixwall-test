#include "CylinderMesh.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

CylinderMeshDataInitializer::CylinderMeshDataInitializer()
{
	GenerateGeometry(0.5f, 1, 32, { 1.0f, 0.6f, 0.2f, 1.0f });
}

void CylinderMeshDataInitializer::GenerateGeometry(float radius, float height, uint32_t segments, std::array<float, 4> color)
{
	if (segments < 3)
		return;
	vertices.emplace_back(0.0f, 0.0f, 0.0f, color[0], color[1], color[2], color[3]);
	vertices.emplace_back(0.0f, 0.0f, height, color[0], color[1], color[2], color[3]);
	for (uint32_t segmentNum = 0; segmentNum < segments; segmentNum++)
	{
		float angle = float(2 * M_PI * segmentNum) / segments;
		float x = -std::sinf(angle) * radius;
		float y = std::cosf(angle) * radius;
		vertices.emplace_back(x, y, 0.0f, color[0], color[1], color[2], color[3]);
		vertices.emplace_back(x, y, height, color[0], color[1], color[2], color[3]);
	}
	for (uint32_t segmentNum = 0; segmentNum < segments; segmentNum++)
	{
		indices.push_back(0);
		indices.push_back(2 + (segmentNum * 2 + 2) % (segments * 2));
		indices.push_back(2 + (segmentNum * 2 + 0) % (segments * 2));
		indices.push_back(1);
		indices.push_back(2 + (segmentNum * 2 + 1) % (segments * 2));
		indices.push_back(2 + (segmentNum * 2 + 3) % (segments * 2));

		indices.push_back(2 + (segmentNum * 2 + 0) % (segments * 2));
		indices.push_back(2 + (segmentNum * 2 + 2) % (segments * 2));
		indices.push_back(2 + (segmentNum * 2 + 1) % (segments * 2));
		indices.push_back(2 + (segmentNum * 2 + 1) % (segments * 2));
		indices.push_back(2 + (segmentNum * 2 + 2) % (segments * 2));
		indices.push_back(2 + (segmentNum * 2 + 3) % (segments * 2));
	}
}

void CylinderMeshDataInitializer::FreeCylinderMeshDataInitializer()
{
	vertices.clear();
	indices.clear();
}

CylinderMesh::CylinderMesh(Render* render) : 
	CylinderMeshDataInitializer(), MeshColored(render, vertices, indices),
	height(1.0f), radius(0.5f), color{ 1.0f, 0.6f, 0.2f, 1.0f }
{
	FreeCylinderMeshDataInitializer();
}

void CylinderMesh::SetPosition(float x, float y, float z)
{
	UpdateMatrixFor3D(DirectX::XMMatrixTranslation(x, y, z));
}

void CylinderMesh::SetHeight(float height)
{
	this->height = height;
}

void CylinderMesh::SetRadius(float radius)
{
	this->radius = radius;
}

void CylinderMesh::SetColor(std::array<float, 4> color)
{
	this->color = color;
}

void CylinderMesh::UpdateGeometry()
{
	CylinderMeshDataInitializer::GenerateGeometry(radius, height, 32, color);
	MeshColored::UpdateGeometry(vertices, indices);
	FreeCylinderMeshDataInitializer();
}
