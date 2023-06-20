#include "ArrowMesh.h"
#include <cmath>
#include <iostream>

ArrowMesh::ArrowMesh(Render* render) :
	MeshColored(render,
		{ TVertexColored{ 0.25f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }, { -0.25f, 0.0f, 0.0f, 0.318f, 0.157f, 0.102f, 1.0f } },
		{ 0, 1 })
{
}

void ArrowMesh::SetPositionAndRotation(float x, float y, float z, float LookAtX, float LookAtY, float LookAtZ)
{
	using namespace DirectX;
	XMVECTOR delta = XMVector3Normalize({ LookAtX - x, LookAtY - y, LookAtZ - z });
	XMVECTOR towardsVector = { 1.0f, 0.0f, 0.0f };
	XMVECTOR rotationQuat = XMQuaternionIdentity();
	float dotResult = XMVectorGetX(XMVector3Dot(delta, towardsVector));
	if (dotResult < -0.999f)
	{
		rotationQuat = XMQuaternionRotationRollPitchYaw(0.0f, XM_PI, 0.0f);
	}
	else if (dotResult <= 0.999f)
	{
		auto crossproduct = XMVector3Normalize(XMVector3Cross(delta, towardsVector));
		rotationQuat = XMQuaternionRotationAxis(crossproduct, -std::acosf(dotResult));
	}
	UpdateMatrixFor3D(DirectX::XMMatrixRotationQuaternion(rotationQuat) * DirectX::XMMatrixTranslation(x, y, z));
}
