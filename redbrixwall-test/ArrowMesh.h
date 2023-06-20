#pragma once

#include "Mesh.h"

class ArrowMesh : public MeshColored
{
public:
	ArrowMesh(Render* render);

	void SetPositionAndRotation(float x, float y, float z, float LookAtX, float LookAtY, float LookAtZ);
};
