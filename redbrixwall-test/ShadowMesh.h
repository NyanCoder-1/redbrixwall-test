#pragma once

#include "Mesh.h"

class ShadowMesh : public MeshTextured
{
public:
	ShadowMesh(Render* render);

	void Draw() override;
	void SetPosition(float x, float y);
};
