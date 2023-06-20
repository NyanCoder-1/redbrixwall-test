#pragma once

#include "Mesh.h"
#include <array>

class CylinderMeshDataInitializer
{
public:
	CylinderMeshDataInitializer();

	void GenerateGeometry(float radius, float height, uint32_t segments, std::array<float, 4> color);

protected:
	void FreeCylinderMeshDataInitializer();
	std::vector<TVertexColored> vertices;
	std::vector<uint32_t> indices;
};

class CylinderMesh : private CylinderMeshDataInitializer, public MeshColored
{
public:
	CylinderMesh(Render* render);

	void SetPosition(float x, float y, float z);

	void SetHeight(float height);
	void SetRadius(float radius);
	void SetColor(std::array<float, 4> color);
	void UpdateGeometry();

private:
	float height;
	float radius;
	std::array<float, 4> color;
};
