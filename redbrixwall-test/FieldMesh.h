#pragma once

#include "Mesh.h"

class FieldMeshDataInitializer
{
public:
	FieldMeshDataInitializer();
protected:
	void FreeFieldMeshDataInitializer();
	std::vector<TVertexColored> vertices;
	std::vector<uint32_t> indices;
};

class FieldMesh : private FieldMeshDataInitializer, public MeshColored
{
public:
	FieldMesh(Render* render);
};
