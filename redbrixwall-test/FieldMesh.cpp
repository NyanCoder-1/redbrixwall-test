#include "FieldMesh.h"

FieldMesh::FieldMesh(Render* render) :
	FieldMeshDataInitializer(), MeshColored(render, FieldMeshDataInitializer::vertices, FieldMeshDataInitializer::indices)
{
	FieldMeshDataInitializer::FreeFieldMeshDataInitializer();
}

FieldMeshDataInitializer::FieldMeshDataInitializer()
{
	for (int32_t y = 0; y < 20; y++)
	{
		for (int32_t x = 0; x < 20; x++)
		{
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 0) * 5 + 0.15f, 0.0f, 0.333f, 0.71f, 0.655f, 1.0f);
			vertices.emplace_back((x + 1) * 5 - 0.15f, (y + 0) * 5 + 0.15f, 0.0f, 0.333f, 0.71f, 0.655f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 1) * 5 - 0.15f, 0.0f, 0.333f, 0.71f, 0.655f, 1.0f);
			vertices.emplace_back((x + 1) * 5 - 0.15f, (y + 1) * 5 - 0.15f, 0.0f, 0.333f, 0.71f, 0.655f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 0) * 5 + 0.15f, -0.0f, 0.071f, 0.306f, 0.271f, 1.0f);
			vertices.emplace_back((x + 1) * 5 - 0.15f, (y + 0) * 5 + 0.15f, -0.0f, 0.071f, 0.306f, 0.271f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 0) * 5 + 0.15f, -0.5f, 0.071f, 0.306f, 0.271f, 1.0f);
			vertices.emplace_back((x + 1) * 5 - 0.15f, (y + 0) * 5 + 0.15f, -0.5f, 0.071f, 0.306f, 0.271f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 0) * 5 + 0.15f, -0.5f, 0.271f, 0.263f, 0.208f, 1.0f);
			vertices.emplace_back((x + 1) * 5 - 0.15f, (y + 0) * 5 + 0.15f, -0.5f, 0.271f, 0.263f, 0.208f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 0) * 5 + 0.15f, -1.0f, 0.271f, 0.263f, 0.208f, 1.0f);
			vertices.emplace_back((x + 1) * 5 - 0.15f, (y + 0) * 5 + 0.15f, -1.0f, 0.271f, 0.263f, 0.208f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 1) * 5 - 0.15f, -0.0f, 0.220f, 0.475f, 0.416f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 0) * 5 + 0.15f, -0.0f, 0.220f, 0.475f, 0.416f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 1) * 5 - 0.15f, -0.5f, 0.220f, 0.475f, 0.416f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 0) * 5 + 0.15f, -0.5f, 0.220f, 0.475f, 0.416f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 1) * 5 - 0.15f, -0.5f, 0.463f, 0.478f, 0.416f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 0) * 5 + 0.15f, -0.5f, 0.463f, 0.478f, 0.416f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 1) * 5 - 0.15f, -1.0f, 0.463f, 0.478f, 0.416f, 1.0f);
			vertices.emplace_back((x + 0) * 5 + 0.15f, (y + 0) * 5 + 0.15f, -1.0f, 0.463f, 0.478f, 0.416f, 1.0f);

			int index = x + y * 20;

			indices.push_back(index * 20 + 0 + 0);
			indices.push_back(index * 20 + 0 + 1);
			indices.push_back(index * 20 + 0 + 2);
			indices.push_back(index * 20 + 0 + 2);
			indices.push_back(index * 20 + 0 + 1);
			indices.push_back(index * 20 + 0 + 3);

			indices.push_back(index * 20 + 4 + 0);
			indices.push_back(index * 20 + 4 + 1);
			indices.push_back(index * 20 + 4 + 2);
			indices.push_back(index * 20 + 4 + 2);
			indices.push_back(index * 20 + 4 + 1);
			indices.push_back(index * 20 + 4 + 3);

			indices.push_back(index * 20 + 8 + 0);
			indices.push_back(index * 20 + 8 + 1);
			indices.push_back(index * 20 + 8 + 2);
			indices.push_back(index * 20 + 8 + 2);
			indices.push_back(index * 20 + 8 + 1);
			indices.push_back(index * 20 + 8 + 3);

			indices.push_back(index * 20 + 12 + 0);
			indices.push_back(index * 20 + 12 + 1);
			indices.push_back(index * 20 + 12 + 2);
			indices.push_back(index * 20 + 12 + 2);
			indices.push_back(index * 20 + 12 + 1);
			indices.push_back(index * 20 + 12 + 3);

			indices.push_back(index * 20 + 16 + 0);
			indices.push_back(index * 20 + 16 + 1);
			indices.push_back(index * 20 + 16 + 2);
			indices.push_back(index * 20 + 16 + 2);
			indices.push_back(index * 20 + 16 + 1);
			indices.push_back(index * 20 + 16 + 3);
		}
	}
}

void FieldMeshDataInitializer::FreeFieldMeshDataInitializer()
{
	vertices.clear();
	indices.clear();
}
